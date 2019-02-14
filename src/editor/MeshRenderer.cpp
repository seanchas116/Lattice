#include "MeshRenderer.hpp"
#include "MeshVAOGenerator.hpp"
#include "MeshPicker.hpp"
#include "../ui/AppState.hpp"
#include "../gl/VAO.hpp"
#include "../document/Document.hpp"
#include "../document/History.hpp"
#include "../document/Mesh.hpp"
#include "../document/MeshItem.hpp"
#include "../support/Debug.hpp"
#include "../support/Camera.hpp"
#include <QMouseEvent>

using namespace glm;

namespace Lattice::Editor {

MeshRenderer::MeshRenderer(const SP<UI::AppState>& appState, const SP<Document::MeshItem> &item) :
    _appState(appState),
    _item(item),
    _meshPicker(makeShared<MeshPicker>(item->mesh())),
    _edgeVAO(makeShared<GL::VAO>()),
    _vertexVAO(makeShared<GL::VAO>())
{
    updateVAOs();
    connect(item->mesh().get(), &Document::Mesh::changed, this, &MeshRenderer::updateVAOs);
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

Opt<Render::HitResult> MeshRenderer::hitTest(dvec2 pos, const SP<Camera> &camera) const {
    LATTICE_OPTIONAL_GUARD(pickResult, _meshPicker->pickFace(_item->location().matrixToWorld(), camera, pos), return {};)
    auto [face, depth] = pickResult;
    Render::HitResult result;
    result.depth = depth;
    return result;
}

void MeshRenderer::mousePress(const Render::MouseEvent &event) {
    glm::dvec3 worldPos = event.worldPos();
    auto [screenDragPos, isInScreen] = event.camera->mapWorldToScreen(worldPos);
    if (!isInScreen) {
        return;
    }

    _dragInitLocation = _item->location();
    _dragInitWorldPos = worldPos;
    _dragStarted = false;

    _appState->document()->selectItem(_item, event.originalEvent->modifiers() & Qt::ShiftModifier);
}

void MeshRenderer::mouseMove(const Render::MouseEvent &event) {
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

    emit updateRequested();
}

}
