#include "Mesh.hpp"

namespace Lattice {
namespace NewMesh {

Mesh::Mesh()
{

}

VertexHandle Mesh::addVertex() {
    auto index = uint32_t(_vertices.size());
    _vertices.push_back(Vertex{});
    return VertexHandle(index);
}

UVPointHandle Mesh::addUVPoint(VertexHandle v) {
    UVPoint uvPoint;
    uvPoint.vertex = v.index;
    auto index = uint32_t(_uvPoints.size());
    _uvPoints.push_back(uvPoint);
    return UVPointHandle(index);
}

EdgeHandle Mesh::addEdge(VertexHandle v0, VertexHandle v1) {
    for (auto edge : edges(v0)) {
        auto edgeVertices = vertices(edge);
        if (edgeVertices == std::array{v0, v1} || edgeVertices == std::array{v1, v0}) {
            // edge already exists
            return edge;
        }
    }

    Edge edge;
    edge.vertices = {v0.index, v1.index};
    auto index = uint32_t(_edges.size());
    _edges.push_back(edge);
    return EdgeHandle(index);
}

} // namespace NewMesh
} // namespace Lattice
