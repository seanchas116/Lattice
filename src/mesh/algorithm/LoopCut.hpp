#pragma once
#include "../Mesh.hpp"

namespace Lattice {
namespace Mesh {

class LoopCut {
public:
    LoopCut(EdgeHandle edge, float cutPosition) : edge(edge), cutPosition(cutPosition) {}

    std::vector<VertexHandle> perform(Mesh& mesh);

    EdgeHandle edge;
    float cutPosition;
};

} // namespace Mesh
} // namespace Lattice
