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

UVPointHandle Mesh::addUVPoint(VertexHandle v) {
    UVPoint uvPoint;
    uvPoint.vertex = v.index();
    uint32_t index = _uvPoints.size();
    _uvPoints.push_back(uvPoint);
    return UVPointHandle(index);
}

} // namespace NewMesh
} // namespace Lattice
