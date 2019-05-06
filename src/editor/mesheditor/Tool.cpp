#include "Tool.hpp"
#include "../../mesh/Mesh.hpp"

namespace Lattice {
namespace Editor {
namespace MeshEditor {

std::unordered_set<Mesh::VertexHandle> Tool::EventTarget::vertices(const Mesh::Mesh &mesh) const {
    std::unordered_set<Mesh::VertexHandle> vertices;
    if (vertex) {
        vertices.insert(*vertex);
    }
    if (edge) {
        vertices.insert(mesh.vertices(*edge)[0]);
        vertices.insert(mesh.vertices(*edge)[1]);
    }
    if (face) {
        for (auto v : mesh.vertices(*face)) {
            vertices.insert(v);
        }
    }

    return vertices;
}

bool Tool::EventTarget::operator==(const Tool::EventTarget &other) const {
    return vertex == other.vertex && edge == other.edge && face == other.face;
}

Tool::~Tool() {
}

Tool::HitTestExclusion Tool::hitTestExclusion() const {
    return {};
}

void Tool::hoverEnterTool(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);
}

void Tool::hoverLeaveTool(const Tool::EventTarget &target) {
    Q_UNUSED(target);
}

void Tool::keyPressTool(QKeyEvent *event) {
    Q_UNUSED(event);
}

void Tool::keyReleaseTool(QKeyEvent *event) {
    Q_UNUSED(event);
}

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
