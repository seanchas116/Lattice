#pragma once

#include "../Mesh.hpp"

namespace Lattice {
namespace Mesh {

std::vector<VertexHandle> extrude(Mesh& mesh, const std::vector<VertexHandle>& vertices, bool addFlipFace = false);

} // namespace Mesh
} // namespace Lattice
