#pragma once
#include "../Mesh.hpp"

namespace Lattice {
namespace Mesh {

std::vector<VertexHandle> findConnected(const std::vector<VertexHandle>& vertices);

} // namespace Mesh
} // namespace Lattice

