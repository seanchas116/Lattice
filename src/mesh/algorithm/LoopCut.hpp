#pragma once
#include "../Mesh.hpp"

namespace Lattice {
namespace Mesh {

std::vector<VertexHandle> loopCut(Mesh& mesh, EdgeHandle edge, float cutPosition);

} // namespace Mesh
} // namespace Lattice
