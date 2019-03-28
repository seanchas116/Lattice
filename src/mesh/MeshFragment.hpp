#pragma once
#include "Mesh.hpp"

namespace Lattice {
namespace Mesh {

class MeshFragment {
public:
    std::unordered_set<SP<Vertex>> vertices;

    bool operator==(const MeshFragment& other) const {
        return vertices == other.vertices;
    }

    bool empty() const { return vertices.empty(); }

    bool contains(const MeshFragment& other) const;

    glm::dvec3 medianPosition() const;

    std::unordered_set<SP<Edge>> edges() const;
    std::unordered_set<SP<Face>> faces() const;
};

}
}
