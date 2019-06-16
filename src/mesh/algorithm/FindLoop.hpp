#pragma once
#include "../Mesh.hpp"

namespace Lattice {
namespace Mesh {

std::vector<EdgeHandle> findLoop(const Mesh& mesh, EdgeHandle edge);

} // namespace Mesh
} // namespace Lattice
