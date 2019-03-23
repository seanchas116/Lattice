#include "MeshRenderer.hpp"
#include "MeshVAOGenerator.hpp"
#include "../state/AppState.hpp"
#include "../gl/VAO.hpp"
#include "../document/Document.hpp"
#include "../document/History.hpp"
#include "../document/MeshItem.hpp"
#include "../mesh/Mesh.hpp"
#include "../support/Debug.hpp"
#include "../support/Camera.hpp"
#include <QMouseEvent>
#include <QMenu>

using namespace glm;

namespace Lattice {
namespace Editor {

MeshRenderer::MeshRenderer(const SP<State::AppState>& appState, const SP<Document::MeshItem> &item) :
    _appState(appState),
    _item(item),
    _edgeVAO(makeShared<GL::VAO>()),
    _vertexVAO(makeShared<GL::VAO>())
{
    updateVAOs();
    connect(item->mesh().get(), &Mesh::Mesh::changed, this, [this] {
        _isVAOsDirty = true;
        update();
    });
}

void MeshRenderer::draw(const SP<Render::Operations> &operations, const SP<Camera> &camera) {
    updateVAOs();
    for (auto& [material, vao] : _faceVAOs) {
        operations->drawMaterial.draw(vao, _item->location().matrixToWorld(), camera, material);
    }
}

void MeshRenderer::drawPickables(const SP<Render::Operations> &operations, const SP<Camera> &camera) {
    updateVAOs();
    for (auto& [material, vao] : _faceVAOs) {
        operations->drawUnicolor.draw(vao, _item->location().matrixToWorld(), camera, toIDColor());
    }
}

void MeshRenderer::mousePress(const Render::MouseEvent &event) {
    switch (event.originalEvent->button()) {
    case Qt::RightButton: {
        QMenu contextMenu;
        contextMenu.addAction(tr("Delete"), _appState.get(), &State::AppState::deleteItems);
        contextMenu.exec(event.originalEvent->globalPos());
        return;
    }
    case Qt::LeftButton: {
        glm::dvec3 worldPos = event.worldPos();
        auto [screenDragPos, isInScreen] = event.camera->mapWorldToScreen(worldPos);
        if (!isInScreen) {
            return;
        }

        _dragged = true;
        _dragInitLocation = _item->location();
        _dragInitWorldPos = worldPos;
        _dragInitScreenPos = event.screenPos;
        _dragStarted = false;

        _appState->document()->selectItem(_item, event.originalEvent->modifiers() & Qt::ShiftModifier);
        return;
    }
    default:
        return;
    }
}

void MeshRenderer::mouseMove(const Render::MouseEvent &event) {
    if (!_dragged) {
        return;
    }

    auto newWorldPos = event.worldPos();
    auto newLocation = _dragInitLocation;
    newLocation.position += newWorldPos - _dragInitWorldPos;

    if (!_dragStarted) {
        if (glm::distance(_dragInitScreenPos, event.screenPos) < _appState->preferences()->moveThreshold()) {
            return;
        }
        _appState->document()->history()->beginChange(tr("Move Item"));
        _dragStarted = true;
    }
    _item->setLocation(newLocation);
}

void MeshRenderer::mouseRelease(const Render::MouseEvent &event) {
    Q_UNUSED(event);
    _dragged = false;
}

void MeshRenderer::mouseDoubleClick(const Render::MouseEvent &event) {
    Q_UNUSED(event);
    _appState->document()->setEditedItem(_item);
}

void MeshRenderer::updateVAOs() {
    if (!_isVAOsDirty) {
        return;
    }

    MeshVAOGenerator generator(_item->mesh());
    _vertexVAO = generator.generateVertexVAO();
    _edgeVAO = generator.generateEdgeVAO();
    _faceVAOs= generator.generateFaceVAOs();

    _isVAOsDirty = false;
}

}
}
