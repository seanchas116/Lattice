#include "Tool.hpp"

namespace Lattice {
namespace Editor {
namespace MeshEditor {

std::unordered_set<SP<Mesh::Vertex> > Tool::EventTarget::vertices() const {
    if (this->vertex) {
        auto& vertex = *this->vertex;
        return {vertex};
    } else if (this->edge) {
        auto& edge = *this->edge;
        return {edge->vertices()[0], edge->vertices()[1]};
    } else if (this->face) {
        auto& face = *this->face;
        std::unordered_set<SP<Mesh::Vertex>> vertices;
        for (auto& v : face->vertices()) {
            vertices.insert(v);
        }
        return vertices;
    }
    return {};
}

Mesh::MeshFragment Tool::EventTarget::fragment() const {
    return Mesh::MeshFragment{vertices()};
}

Tool::~Tool() {
}

Tool::HitTestExclusion Tool::hitTestExclusion() const {
    return {};
}

void Tool::hoverEnterEvent(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);
}

void Tool::hoverLeaveEvent(const Tool::EventTarget &target) {
    Q_UNUSED(target);
}

void Tool::keyPressEvent(QKeyEvent *event) {
    Q_UNUSED(event);
}

void Tool::keyReleaseEvent(QKeyEvent *event) {
    Q_UNUSED(event);
}

void Tool::draw(const SP<Draw::Operations> &operations, const SP<Camera> &camera) {
    Q_UNUSED(operations); Q_UNUSED(camera);
}

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
