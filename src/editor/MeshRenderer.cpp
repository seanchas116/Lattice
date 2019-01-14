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
    updateVAOs(item->mesh());
    connect(_item.get(), &Document::MeshItem::meshChanged, this, &MeshRenderer::updateVAOs);
}

void MeshRenderer::draw(const SP<Render::Operations> &operations, const Camera &camera) {
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

std::optional<Render::HitResult> MeshRenderer::hitTest(dvec2 pos, const Camera &camera) const {
    LATTICE_OPTIONAL_GUARD(pickResult, _meshPicker->picKFace(_item->location().matrixToModel(), camera, pos), return {};)
    auto [face, t] = pickResult;
    Render::HitResult result;
    result.t = t;
    return result;
}

void MeshRenderer::mousePress(const Render::MouseEvent &event) {
    glm::dvec3 worldDragPos = event.camera.worldMouseRay(event.screenPos).at(event.hitResult.t);
    auto [screenDragPos, isInScreen] = event.camera.mapWorldToScreen(worldDragPos);
    if (!isInScreen) {
        return;
    }

    _dragInitLocation = _item->location();
    _dragInitWorldPos = worldDragPos;
    _dragInitDepth = screenDragPos.z;
    _dragStarted = false;

    _appState->document()->selectItem(_item, event.originalEvent->modifiers() & Qt::ShiftModifier);
}

void MeshRenderer::mouseMove(const Render::MouseEvent &event) {
    auto newWorldPos = event.camera.mapScreenToWorld(glm::dvec3(event.screenPos, _dragInitDepth));
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

void MeshRenderer::updateVAOs(const SP<Document::Mesh> &mesh) {
    MeshVAOGenerator generator(mesh);
    _vertexVAO = generator.generateVertexVAO();
    _edgeVAO = generator.generateEdgeVAO();
    _faceVAOs= generator.generateFaceVAOs();
}

}
