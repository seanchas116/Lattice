#include "MeshRenderer.hpp"
#include "MeshVAOGenerator.hpp"
#include "MeshPicker.hpp"
#include "../ui/AppState.hpp"
#include "../gl/LineVAO.hpp"
#include "../gl/PointVAO.hpp"
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
    _edgeVAO(makeShared<GL::LineVAO>()),
    _vertexVAO(makeShared<GL::PointVAO>())
{
    // TODO: update mesh when item is changed
    updateVAOs(item->mesh());
}

void MeshRenderer::draw(const SP<Render::Operations> &operations, const Camera &camera) {
    if (_appState->isFaceVisible()) {
        for (auto& [material, vao] : _faceVAOs) {
            operations->drawMaterial.draw(vao, _item->location().matrix(), camera, material);
        }
    }
    if (_appState->isEdgeVisible()) {
        operations->drawLine.draw(_edgeVAO, _item->location().matrix(), camera, 1.0, dvec3(0));
    }
    if (_appState->isVertexVisible()) {
        operations->drawCircle.draw(_vertexVAO, _item->location().matrix(), camera, 4.0, dvec3(0));
    }
}

std::pair<bool, double> MeshRenderer::mousePress(QMouseEvent *event, dvec2 pos, const Camera &camera) {
    auto worldMouseRay = camera.worldMouseRay(pos);
    LATTICE_OPTIONAL_GUARD(pickResult, _meshPicker->picKFace(worldMouseRay), return {true, 0};)
    auto [face, t] = pickResult;

    glm::dvec3 worldDragPos = worldMouseRay.at(t);
    auto [screenDragPos, isInScreen] = camera.mapWorldToScreen(worldDragPos);
    if (!isInScreen) {
        return {false, 0};
    }

    _dragInitLocation = _item->location();
    _dragInitWorldPos = worldDragPos;
    _dragInitDepth = screenDragPos.z;
    _dragStarted = false;

    _appState->document()->selectItem(_item, event->modifiers() & Qt::ShiftModifier);

    return {true, 0};
}

void MeshRenderer::mouseMove(QMouseEvent *event, dvec2 pos, const Camera &camera) {
    Q_UNUSED(event);

    auto newWorldPos = camera.mapScreenToWorld(glm::dvec3(pos, _dragInitDepth));
    auto newLocation = _dragInitLocation;
    newLocation.position += newWorldPos - _dragInitWorldPos;

    if (!_dragStarted) {
        _appState->document()->history()->beginChange(tr("Move Item"));
        _dragStarted = true;
    }
    _item->setLocation(newLocation);
}

void MeshRenderer::mouseRelease(QMouseEvent *event, dvec2 pos, const Camera &camera) {
    Q_UNUSED(event); Q_UNUSED(pos); Q_UNUSED(camera);
}

void MeshRenderer::updateVAOs(const SP<Document::Mesh> &mesh) {
    MeshVAOGenerator generator(mesh);
    _vertexVAO = generator.generateVertexVAO();
    _edgeVAO = generator.generateEdgeVAO();
    _faceVAOs= generator.generateFaceVAOs();
}

}
