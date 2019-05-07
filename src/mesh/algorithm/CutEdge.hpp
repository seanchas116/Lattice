#pragma once
#include "../Mesh.hpp"

namespace Lattice {
namespace Mesh {

class CutEdge {
public:
    // TODO: use persistent handle
    CutEdge(EdgeHandle edge, float t) : edge(edge), t(t) {}
    VertexHandle perform(Mesh& mesh) const;

private:
    EdgeHandle edge;
    float t;
};

} // namespace Mesh
} // namespace Lattice

