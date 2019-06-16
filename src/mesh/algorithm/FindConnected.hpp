#pragma once
#include "../Mesh.hpp"

namespace Lattice {
namespace Mesh {

std::unordered_set<VertexHandle> findConnected(const Mesh& mesh, const std::vector<VertexHandle>& vertices);

} // namespace Mesh
} // namespace Lattice

