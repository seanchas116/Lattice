#include "Mesh.hpp"

namespace Lattice {
namespace NewMesh {

VertexHandle Mesh::addVertex() {
    auto index = uint32_t(_vertices.size());
    _vertices.push_back(Vertex{});
    return VertexHandle(index);
}

UVPointHandle Mesh::addUVPoint(VertexHandle v) {
    UVPoint uvPoint;
    uvPoint.vertex = v;
    auto index = uint32_t(_uvPoints.size());
    _uvPoints.push_back(uvPoint);
    return UVPointHandle(index);
}

EdgeHandle Mesh::addEdge(VertexHandle v0, VertexHandle v1) {
    // check if edge already exists
    for (auto edge : edges(v0)) {
        auto edgeVertices = vertices(edge);
        if (edgeVertices == std::array{v0, v1} || edgeVertices == std::array{v1, v0}) {
            // same edge found
            return edge;
        }
    }

    Edge edge;
    edge.vertices = {v0, v1};
    auto index = uint32_t(_edges.size());
    _edges.push_back(edge);
    return EdgeHandle(index);
}

FaceHandle Mesh::addFace(const std::vector<UVPointHandle> &uvPoints) {
    // check if face already exists
    for (auto face : faces(uvPoints[0])) {
        auto faceUVPoints = this->uvPoints(face) | ranges::to_vector;
        if (faceUVPoints.size() != uvPoints.size()) {
            continue;
        }
        for (size_t offset = 0; offset < uvPoints.size(); ++offset) {
            bool isSame = true;
            for (size_t i = 0; i < uvPoints.size(); ++i) {
                if (faceUVPoints[(i + offset) % uvPoints.size()] != uvPoints[i]) {
                    isSame = false;
                    break;
                }
            }
            if (isSame) {
                // same face found
                return face;
            }
        }
    }

    Face face;
    face.uvPoints = uvPoints;

    for (size_t i = 0; i < uvPoints.size(); ++i) {
        auto uv0 = uvPoints[i];
        auto uv1 = uvPoints[(i + 1) % uvPoints.size()];
        face.edges.push_back(addEdge(vertex(uv0), vertex(uv1)));
    }

    auto index = uint32_t(_faces.size());
    _faces.push_back(face);
    return FaceHandle(index);
}

} // namespace NewMesh
} // namespace Lattice
