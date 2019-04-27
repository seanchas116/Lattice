#include "Mesh.hpp"

namespace Lattice {
namespace NewMesh {

Mesh::Mesh()
{

}

VertexHandle Mesh::addVertex() {
    uint32_t index = _vertices.size();
    _vertices.push_back(Vertex{});
    return VertexHandle(index);
}

} // namespace NewMesh
} // namespace Lattice
