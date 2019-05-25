#pragma once
#include "../Mesh.hpp"

namespace Lattice {
namespace Mesh {

class FindBelt {
public:
    FindBelt(EdgeHandle edge) : edge(edge) {}

    struct BeltElement {
        EdgeHandle edge;
        FaceHandle face;
        bool isEdgeReverse;
    };

    std::vector<BeltElement> perform(const Mesh& mesh) const;

    EdgeHandle edge;
};

} // namespace Mesh
} // namespace Lattice
