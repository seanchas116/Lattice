#pragma once
#include "../mesh/Mesh.hpp"

namespace Lattice {
namespace Document {

class MeshSelection {
public:
    std::unordered_set<SP<Mesh::Vertex>> vertices;

    bool operator==(const MeshSelection& other) const {
        return vertices == other.vertices;
    }

    glm::vec3 medianPosition() const;

    std::unordered_set<SP<Mesh::Edge>> edges() const;
    std::unordered_set<SP<Mesh::Face>> faces() const;
};

} // namespace Document
} // namespace Lattice
