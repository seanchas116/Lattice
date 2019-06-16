#pragma once
#include "../Mesh.hpp"

namespace Lattice {
namespace Mesh {

struct BeltElement {
    EdgeHandle edge;
    FaceHandle face;
    bool isEdgeReverse;
};

std::vector<BeltElement> findBelt(const Mesh& mesh, EdgeHandle edge);

} // namespace Mesh
} // namespace Lattice
