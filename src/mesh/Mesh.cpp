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
    uvPointData.position = position;
    uvPointData.vertex = v;
    auto uvPoint = UVPointHandle(uint32_t(_uvPoints.size()));
    _uvPoints.push_back(uvPointData);
    _vertices[v.index].uvPoints.push_back(uvPoint);
    return uvPoint;
}

EdgeHandle Mesh::addEdge(VertexHandle v0, VertexHandle v1, bool isSmooth) {
    // check if edge already exists
    for (auto edge : edges(v0)) {
        auto edgeVertices = vertices(edge);
        if (edgeVertices == std::array{v0, v1} || edgeVertices == std::array{v1, v0}) {
            // same edge found
            return edge;
        }
    }

    EdgeData edgeData;
    edgeData.isSmooth = isSmooth;
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

void Mesh::collectGarbage() {
    {
        std::vector<VertexData> newVertices;
        for (size_t i = 0; i < _vertices.size(); ++i) {
            auto& vertexData = _vertices[i];
            if (vertexData.isDeleted) {
                continue;
            }
            size_t newIndex = newVertices.size();
            for (auto p : vertexData.uvPoints) {
                if (uvPointData(p).vertex.index == i) {
                    uvPointData(p).vertex.index = uint32_t(newIndex);
                }
            }
            for (auto e : vertexData.edges) {
                for (auto& v : edgeData(e).vertices) {
                    if (v.index == i) {
                        v.index = uint32_t(newIndex);
                    }
                }
            }

            newVertices.push_back(vertexData);
        }
        _vertices = std::move(newVertices);
    }

    {
        std::vector<UVPointData> newUVPoints;
        for (size_t i = 0; i < _uvPoints.size(); ++i) {
            auto& uvPointData = _uvPoints[i];
            if (uvPointData.isDeleted) {
                continue;
            }
            size_t newIndex = newUVPoints.size();
            for (auto& p : vertexData(uvPointData.vertex).uvPoints) {
                if (p.index == i) {
                    p.index = uint32_t(newIndex);
                }
            }
            for (auto f : uvPointData.faces) {
                for (auto& p : faceData(f).uvPoints) {
                    if (p.index == i) {
                        p.index = uint32_t(newIndex);
                    }
                }
            }
        }
        _uvPoints = std::move(newUVPoints);
    }

    {
        std::vector<EdgeData> newEdges;
        for (size_t i = 0; i < _edges.size(); ++i) {
            auto& edgeData = _edges[i];
            if (edgeData.isDeleted) {
                continue;
            }
            size_t newIndex = newEdges.size();
            for (auto& v : edgeData.vertices) {
                for (auto& e : vertexData(v).edges) {
                    if (e.index == i) {
                        e.index = uint32_t(newIndex);
                    }
                }
            }
            for (auto& f : edgeData.faces) {
                for (auto& e : faceData(f).edges) {
                    if (e.index == i) {
                        e.index = uint32_t(newIndex);
                    }
                }
            }
        }
        _edges = std::move(newEdges);
    }

    {
        std::vector<FaceData> newFaces;
        for (size_t i = 0; i < _faces.size(); ++i) {
            auto& faceData = _faces[i];
            if (faceData.isDeleted) {
                continue;
            }
            size_t newIndex = newFaces.size();
            for (auto& uv : faceData.uvPoints) {
                for (auto& f : uvPointData(uv).faces) {
                    if (f.index == i) {
                        f.index = uint32_t(newIndex);
                    }
                }
            }
            for (auto& e : faceData.edges) {
                for (auto& f : edgeData(e).faces) {
                    if (f.index == i) {
                        f.index = uint32_t(newIndex);
                    }
                }
            }
        }
        _faces = std::move(newFaces);
    }
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

void Mesh::clearSelections() {
    for (auto v : vertices()) {
        setSelected(v, false);
    }
}

} // namespace NewMesh
} // namespace Lattice
