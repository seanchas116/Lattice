#pragma once
#include "Mesh.hpp"

namespace Lattice {
namespace Document {

class MeshSelection {
public:
    std::unordered_set<SP<MeshVertex>> vertices;
    bool operator==(const MeshSelection& other) const {
        return vertices == other.vertices;
    }
};

} // namespace Document
} // namespace Lattice
