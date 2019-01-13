#include "EditedMeshRenderer.hpp"
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

EditedMeshRenderer::EditedMeshRenderer(const SP<UI::AppState>& appState, const SP<Document::MeshItem> &item) :
    _appState(appState),
    _item(item),
    _meshPicker(makeShared<MeshPicker>(item->mesh())),
    _edgeVAO(makeShared<GL::LineVAO>()),
    _vertexVAO(makeShared<GL::PointVAO>())
{
    // TODO: update mesh when item is changed
    updateVAOs(item->mesh());
}

void EditedMeshRenderer::draw(const SP<Render::Operations> &operations, const Camera &camera) {
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

std::optional<Render::HitResult> EditedMeshRenderer::hitTest(dvec2 pos, const Camera &camera) const {
    auto worldMouseRay = camera.worldMouseRay(pos);
    auto modelMouseRay = glm::inverse(_item->location().matrix()) * worldMouseRay;
    LATTICE_OPTIONAL_GUARD(pickResult, _meshPicker->picKFace(modelMouseRay), return {};)
    auto [face, t] = pickResult;
    return {{t}};
}

void EditedMeshRenderer::mousePress(QMouseEvent *event, dvec2 pos, const Camera &camera, const Render::HitResult& hitResult) {
    Q_UNUSED(event); Q_UNUSED(pos); Q_UNUSED(camera); Q_UNUSED(hitResult);
}

void EditedMeshRenderer::mouseMove(QMouseEvent *event, dvec2 pos, const Camera &camera) {
    Q_UNUSED(event); Q_UNUSED(pos); Q_UNUSED(camera);
}

void EditedMeshRenderer::mouseRelease(QMouseEvent *event, dvec2 pos, const Camera &camera) {
    Q_UNUSED(event); Q_UNUSED(pos); Q_UNUSED(camera);
}

void EditedMeshRenderer::updateVAOs(const SP<Document::Mesh> &mesh) {
    MeshVAOGenerator generator(mesh);
    _vertexVAO = generator.generateVertexVAO();
    _edgeVAO = generator.generateEdgeVAO();
    _faceVAOs= generator.generateFaceVAOs();
}

}
