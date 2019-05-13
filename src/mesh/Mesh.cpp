#include "Mesh.hpp"

namespace Lattice {
namespace Mesh {

VertexHandle Mesh::addVertex(glm::vec3 position) {
    VertexData vertexData;
    vertexData.position = position;
    auto vertex = VertexHandle(uint32_t(_vertices.size()));
    _vertices.push_back(vertexData);
    return vertex;
}

UVPointHandle Mesh::addUVPoint(VertexHandle v, glm::vec2 position) {
    UVPointData uvPointData;
    uvPointData.uvPosition = position;
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

FaceHandle Mesh::addFace(const std::vector<UVPointHandle> &uvPoints, uint32_t material) {
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
    faceData.material = material;
    faceData.uvPoints = uvPoints;

    for (size_t i = 0; i < uvPoints.size(); ++i) {
        auto uv0 = uvPoints[i];
        auto uv1 = uvPoints[(i + 1) % uvPoints.size()];
        faceData.edges.push_back(addEdge(vertex(uv0), vertex(uv1), true));
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
    for (auto uv : vertexData(v).uvPoints) {
        removeUVPoint(uv);
    }
    for (auto e : vertexData(v).edges) {
        removeEdge(e);
    }
    vertexData(v).isDeleted = true;
}

void Mesh::removeUVPoint(UVPointHandle uv) {
    for (auto f : uvPointData(uv).faces) {
        removeFace(f);
    }
    uvPointData(uv).isDeleted = true;
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

Mesh Mesh::collectGarbage() const {
    std::vector<VertexData> newVertices;
    std::vector<uint32_t> newVertexIndices(_vertices.size());

    for (size_t i = 0; i < _vertices.size(); ++i) {
        auto& vertexData = _vertices[i];
        if (vertexData.isDeleted) {
            continue;
        }
        newVertexIndices[i] = uint32_t(newVertices.size());
        newVertices.push_back(vertexData);
    }

    std::vector<UVPointData> newUVPoints;
    std::vector<uint32_t> newUVPointIndices(_uvPoints.size());
    for (size_t i = 0; i < _uvPoints.size(); ++i) {
        auto& uvPointData = _uvPoints[i];
        if (uvPointData.isDeleted) {
            continue;
        }
        newUVPointIndices[i] = uint32_t(newUVPoints.size());
        newUVPoints.push_back(uvPointData);
    }

    std::vector<EdgeData> newEdges;
    std::vector<uint32_t> newEdgeIndices(_edges.size());
    for (size_t i = 0; i < _edges.size(); ++i) {
        auto& edgeData = _edges[i];
        if (edgeData.isDeleted) {
            continue;
        }
        newEdgeIndices[i] = uint32_t(newEdges.size());
        newEdges.push_back(edgeData);
    }

    std::vector<FaceData> newFaces;
    std::vector<uint32_t> newFaceIndices(_faces.size());
    for (size_t i = 0; i < _faces.size(); ++i) {
        auto& faceData = _faces[i];
        if (faceData.isDeleted) {
            continue;
        }
        newFaceIndices[i] = uint32_t(newFaces.size());
        newFaces.push_back(faceData);
    }

    for (auto& vertexData : newVertices) {
        for (auto& uvPoint : vertexData.uvPoints) {
            uvPoint.index = newUVPointIndices[uvPoint.index];
        }
        for (auto& edge : vertexData.edges) {
            edge.index = newEdgeIndices[edge.index];
        }
    }
    for (auto& uvPointData : newUVPoints) {
        uvPointData.vertex.index = newVertexIndices[uvPointData.vertex.index];
        for (auto& face : uvPointData.faces) {
            face.index = newFaceIndices[face.index];
        }
    }
    for (auto& edgeData  : newEdges) {
        for (auto& vertex : edgeData.vertices) {
            vertex.index = newVertexIndices[vertex.index];
        }
        for (auto& face : edgeData.faces) {
            face.index = newFaceIndices[face.index];
        }
    }
    for (auto& faceData : newFaces) {
        for (auto& uvPoint : faceData.uvPoints) {
            uvPoint.index = newUVPointIndices[uvPoint.index];
        }
        for (auto& edge : faceData.edges) {
            edge.index = newEdgeIndices[edge.index];
        }
    }

    Mesh mesh;
    mesh._vertices = std::move(newVertices);
    mesh._uvPoints = std::move(newUVPoints);
    mesh._edges = std::move(newEdges);
    mesh._faces = std::move(newFaces);
    return mesh;
}

void Mesh::clear() {
    _vertices.clear();
    _uvPoints.clear();
    _edges.clear();
    _faces.clear();
}

glm::vec3 Mesh::calculateNormal(FaceHandle face) const {
    auto vertices = this->vertices(face);

    if (vertices.size() == 3) {
        return normalize(cross(position(vertices[1]) - position(vertices[0]), position(vertices[2]) - position(vertices[0])));
    }

    // find average vertex normal
    glm::vec3 normalSum(0);
    int sumCount = 0;
    int vertexCount = int(vertices.size());

    for (int i = 0; i < vertexCount; ++i) {
        auto prev = position(vertices[i]);
        auto curr = position(vertices[(i + 1) % vertexCount]);
        auto next = position(vertices[(i + 2) % vertexCount]);
        auto crossValue = cross(next- curr, prev - curr);
        if (crossValue == glm::vec3(0)) {
            continue;
        }
        auto normal = normalize(crossValue);
        normalSum += normal;
        ++sumCount;
    }
    if (sumCount == 0) {
        return glm::vec3(0); // TODO: what should we do?
    }
    return normalize(normalSum);
}

void Mesh::selectAll() {
    for (auto v : vertices()) {
        setSelected(v, true);
    }
}

void Mesh::deselectAll() {
    for (auto v : vertices()) {
        setSelected(v, false);
    }
}

void Mesh::merge(const Mesh &other) {
    auto vertexOffset = uint32_t(_vertices.size());
    auto uvPointOffset = uint32_t(_uvPoints.size());
    auto edgeOffset = uint32_t(_edges.size());
    auto faceOffset = uint32_t(_faces.size());

    _vertices.reserve(_vertices.size() + other._vertices.size());
    _uvPoints.reserve(_uvPoints.size() + other._uvPoints.size());
    _edges.reserve(_edges.size() + other._edges.size());
    _faces.reserve(_faces.size() + other._faces.size());

    for (auto vertexData : other._vertices) {
        for (auto& uv : vertexData.uvPoints) {
            uv.index += uvPointOffset;
        }
        for (auto& e : vertexData.edges) {
            e.index += edgeOffset;
        }
        _vertices.push_back(vertexData);
    }
    for (auto uvPointData : other._uvPoints) {
        uvPointData.vertex.index += vertexOffset;
        for (auto& f : uvPointData.faces) {
            f.index += faceOffset;
        }
        _uvPoints.push_back(uvPointData);
    }
    for (auto edgeData : other._edges) {
        for (auto& v : edgeData.vertices) {
            v.index += vertexOffset;
        }
        for (auto& f : edgeData.faces) {
            f.index += faceOffset;
        }
        _edges.push_back(edgeData);
    }
    for (auto faceData : other._faces) {
        for (auto& uv : faceData.uvPoints) {
            uv.index += uvPointOffset;
        }
        for (auto& e : faceData.edges) {
            e.index += edgeOffset;
        }
        _faces.push_back(faceData);
    }
}

} // namespace NewMesh
} // namespace Lattice
