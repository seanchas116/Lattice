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

namespace Lattice::Editor {

MeshRenderer::MeshRenderer(const SP<State::AppState>& appState, const SP<Document::MeshItem> &item) :
    _appState(appState),
    _item(item),
    _edgeVAO(makeShared<GL::VAO>()),
    _vertexVAO(makeShared<GL::VAO>())
{
    updateVAOs();
    connect(item->mesh().get(), &Mesh::Mesh::changed, this, &MeshRenderer::updateVAOs);
}

void MeshRenderer::draw(const SP<Render::Operations> &operations, const SP<Camera> &camera) {
    if (_appState->isFaceVisible()) {
        for (auto& [material, vao] : _faceVAOs) {
            operations->drawMaterial.draw(vao, _item->location().matrixToWorld(), camera, material);
        }
    }
    /*
    if (_appState->isEdgeVisible()) {
        operations->drawLine.draw(_edgeVAO, _item->location().matrix(), camera, 1.0, dvec3(0));
    }
    if (_appState->isVertexVisible()) {
        operations->drawCircle.draw(_vertexVAO, _item->location().matrix(), camera, 4.0, dvec3(0));
    }
    */
}

void MeshRenderer::drawPickables(const SP<Render::Operations> &operations, const SP<Camera> &camera) {
    if (_appState->isFaceVisible()) {
        for (auto& [material, vao] : _faceVAOs) {
            operations->drawUnicolor.draw(vao, _item->location().matrixToWorld(), camera, toIDColor());
        }
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
    recallContext();

    MeshVAOGenerator generator(_item->mesh());
    _vertexVAO = generator.generateVertexVAO();
    _edgeVAO = generator.generateEdgeVAO();
    _faceVAOs= generator.generateFaceVAOs();

    update();
}

}
