#include "Mesh.hpp"

namespace Lattice {
namespace NewMesh {

VertexHandle Mesh::addVertex() {
    auto vertex = VertexHandle(uint32_t(_vertices.size()));
    _vertices.push_back(VertexData{});
    return vertex;
}

UVPointHandle Mesh::addUVPoint(VertexHandle v) {
    UVPointData uvPointData;
    uvPointData.vertex = v;
    auto uvPoint = UVPointHandle(uint32_t(_uvPoints.size()));
    _uvPoints.push_back(uvPointData);
    _vertices[v.index].uvPoints.push_back(uvPoint);
    return uvPoint;
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

    EdgeData edgeData;
    edgeData.vertices = {v0, v1};
    auto edge = EdgeHandle(uint32_t(_edges.size()));
    _edges.push_back(edgeData);
    vertexData(v0).edges.push_back(edge);
    vertexData(v1).edges.push_back(edge);
    return edge;
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

    FaceData faceData;
    faceData.uvPoints = uvPoints;

    for (size_t i = 0; i < uvPoints.size(); ++i) {
        auto uv0 = uvPoints[i];
        auto uv1 = uvPoints[(i + 1) % uvPoints.size()];
        faceData.edges.push_back(addEdge(vertex(uv0), vertex(uv1)));
    }

    auto face = FaceHandle(uint32_t(_faces.size()));
    _faces.push_back(faceData);
    for (auto uvPoint : faceData.uvPoints) {
        uvPointData(uvPoint).faces.push_back(face);
    }
    for (auto edge : faceData.edges) {
        edgeData(edge).faces.push_back(face);
    }
    return face;
}

void Mesh::removeVertex(VertexHandle v) {
    for (auto e : vertexData(v).edges) {
        removeEdge(e);
    }
    vertexData(v).isDeleted = true;
}

void Mesh::removeEdge(EdgeHandle e) {
    for (auto f : edgeData(e).faces) {
        removeFace(f);
    }
    edgeData(e).isDeleted = true;
}

void Mesh::removeFace(FaceHandle f) {
    faceData(f).isDeleted = true;
}

} // namespace NewMesh
} // namespace Lattice
