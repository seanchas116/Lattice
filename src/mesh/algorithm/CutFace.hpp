#pragma once
#include "../Mesh.hpp"

namespace Lattice {
namespace Mesh {

class CutFace {
public:
    CutFace(EdgeHandle edge) : edge(edge) {}
    void perform(Mesh& mesh);

private:
    EdgeHandle edge;
};

} // namespace Mesh
} // namespace Lattice
