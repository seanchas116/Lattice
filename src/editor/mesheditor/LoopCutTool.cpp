#include "LoopCutTool.hpp"
#include "../../support/Distance.hpp"
#include <meshlib/Mesh.hpp>
#include <meshlib/algorithm/LoopCut.hpp>
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
    auto &mesh = *this->mesh();

    auto edgeRay = Ray<double>::fromPoints(mesh.positions(edge));
    Ray<double> mouseRay = event.camera.modelMouseRay(object()->location().matrixToWorld(), event.viewportPos);
    RayRayDistanceSolver distanceSolver(edgeRay, mouseRay);
    float cutPosition = distanceSolver.t0;

    auto vertices = meshlib::loopCut(mesh, edge, cutPosition);
    if (vertices.empty()) {
        return;
    }

    mesh.deselectAll();
    for (auto &v : vertices) {
        mesh.setSelected(v, true);
    }
    meshEditState()->commitMeshChanged(tr("Loop Cut"));
}

void LoopCutTool::mouseMoveTool(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    Q_UNUSED(target);
    Q_UNUSED(event);
    // TODO
}

void LoopCutTool::mouseReleaseTool(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    Q_UNUSED(target);
    Q_UNUSED(event);
    emit finished();
}

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
