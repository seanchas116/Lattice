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
    auto pickResult = _meshPicker->pickVertex(modelMouseRay, 0.1); // TODO: tweak hit distance based on depth
    if (!pickResult) {
        return std::nullopt;
    }
    auto [vertex, t] = *pickResult;
    Render::HitResult result;
    result.t = t;
    result.vertex = vertex;
    return result;
}

void EditedMeshRenderer::mousePress(const Render::MouseEvent &event) {
    qDebug() << "vertex click at" << event.hitResult.vertex;
}

void EditedMeshRenderer::mouseMove(const Render::MouseEvent &event) {
    Q_UNUSED(event);
}

void EditedMeshRenderer::mouseRelease(const Render::MouseEvent &event) {
    Q_UNUSED(event);
}

void EditedMeshRenderer::updateVAOs(const SP<Document::Mesh> &mesh) {
    MeshVAOGenerator generator(mesh);
    _vertexVAO = generator.generateVertexVAO();
    _edgeVAO = generator.generateEdgeVAO();
    _faceVAOs= generator.generateFaceVAOs();
}

}
