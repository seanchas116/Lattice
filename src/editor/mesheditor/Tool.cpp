#include "Tool.hpp"

namespace Lattice {
namespace Editor {
namespace MeshEditor {

Tool::~Tool() {
}

Tool::HitTestExclusion Tool::hitTestExclusion() const {
    return {};
}

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

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
