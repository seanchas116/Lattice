#include "ExtrudeTool.hpp"
#include "../../document/Document.hpp"
#include "../../document/History.hpp"

using namespace glm;

namespace Lattice {
namespace Editor {
namespace MeshEditor {

Tool::HitTestExclusion ExtrudeTool::hitTestExclusion() const {
    return {};
}

void ExtrudeTool::mousePress(const Tool::EventTarget &target, const Render::MouseEvent &event) {
    // TODO: start extruding when dragg started

    auto& mesh = item()->mesh();

    if (target.vertex) {
        appState()->document()->history()->beginChange(tr("Extrude"));

        // vertex extrude
        auto vertex = *target.vertex;
        auto newUVPoint = mesh->addUVPoint(mesh->addVertex(vertex->position()), vec2(0));
        mesh->addEdge({vertex, newUVPoint->vertex()});
        _oldToNewVertices.insert({vertex, newUVPoint->vertex()});
    }

    for (auto& [oldVertex, newVertex] : _oldToNewVertices) {
        _initPositions[oldVertex] = oldVertex->position();
    }
    _initWorldPos = event.worldPos();
}

void ExtrudeTool::mouseMove(const Tool::EventTarget &target, const Render::MouseEvent &event) {
    Q_UNUSED(target);

    auto offset = event.worldPos() - _initWorldPos;

    std::unordered_map<SP<Mesh::Vertex>, vec3> newPositions;

    for (auto& [oldVertex, newVertex] : _oldToNewVertices) {
        newPositions[newVertex] = _initPositions[oldVertex] + offset;
    }

    item()->mesh()->setPositions(newPositions);
}

void ExtrudeTool::mouseRelease(const Tool::EventTarget &target, const Render::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);

    _oldToNewVertices.clear();
    _initPositions.clear();
}

void ExtrudeTool::hoverEnter(const Tool::EventTarget &target, const Render::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);
    // TOOD
}

void ExtrudeTool::hoverLeave(const Tool::EventTarget &target) {
    Q_UNUSED(target);
    // TOOD
}

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
