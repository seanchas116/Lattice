#include "LoopCutTool.hpp"
#include "../../mesh/Mesh.hpp"
#include "../../mesh/algorithm/LoopCut.hpp"
#include "../../support/Distance.hpp"
#include <range/v3/algorithm/find.hpp>
#include <range/v3/algorithm/find_if.hpp>

using namespace glm;

namespace Lattice {
namespace Editor {
namespace MeshEditor {

void LoopCutTool::mousePressTool(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    if (!target.edge) {
        return;
    }
    auto edge = *target.edge;
    auto& mesh = *this->mesh();

    Ray<double> mouseRay = event.camera->modelMouseRay(object()->location().matrixToWorld(), event.viewportPos);
    RayRayDistanceSolver distanceSolver(Ray<double>(mesh.ray(edge)), mouseRay);
    float cutPosition = distanceSolver.t0;

    auto vertices = Mesh::LoopCut(edge, cutPosition).perform(mesh);
    if (vertices.empty()) {
        return;
    }

    mesh.deselectAll();
    for (auto& v : vertices) {
        mesh.setSelected(v, true);
    }
    meshEditState()->commitMeshChanged(tr("Loop Cut"));
}

void LoopCutTool::mouseMoveTool(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);
    // TODO
}

void LoopCutTool::mouseReleaseTool(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);
    emit finished();
}

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
