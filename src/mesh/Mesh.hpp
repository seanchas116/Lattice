#pragma once
#include <vector>
#include <array>
#include <glm/glm.hpp>
#include <range/v3/all.hpp>

namespace Lattice {
namespace Mesh {

class Mesh;

template <typename TDerived>
class Handle {
public:
    explicit Handle(uint32_t index) : index(index) {}
    bool operator==(const TDerived& other) const { return index == other.index; }
    bool operator!=(const TDerived& other) const { return !operator==(other); }
    uint32_t index;
};

class VertexHandle : public Handle<VertexHandle> {
public:
    VertexHandle() : Handle(0) {}
    explicit VertexHandle(uint32_t index) : Handle(index) {}
};

class UVPointHandle : public Handle<UVPointHandle> {
public:
    UVPointHandle() : Handle(0) {}
    explicit UVPointHandle(uint32_t index) : Handle(index) {}
};

class EdgeHandle : public Handle<EdgeHandle> {
public:
    EdgeHandle() : Handle(0) {}
    EdgeHandle(uint32_t index) : Handle(index) {}
};

class FaceHandle : public Handle<FaceHandle> {
public:
    FaceHandle() : Handle(0) {}
    FaceHandle(uint32_t index) : Handle(index) {}
};


class VertexData {
public:
    bool isDeleted = false;
    glm::vec3 position;
    std::vector<UVPointHandle> uvPoints;
    std::vector<EdgeHandle> edges;
};

class UVPointData {
public:
    bool isDeleted = false;
    glm::vec2 position;
    VertexHandle vertex;
    std::vector<FaceHandle> faces;
};

class EdgeData {
public:
    bool isDeleted = false;
    bool isSmooth = true;
    std::array<VertexHandle, 2> vertices;
    std::vector<FaceHandle> faces;
};

class FaceData {
public:
    bool isDeleted = false;
    std::vector<UVPointHandle> uvPoints;
    std::vector<EdgeHandle> edges;
    int material;
};


class Mesh {
    auto& vertexData(VertexHandle handle) { return _vertices[handle.index]; }
    auto& vertexData(VertexHandle handle) const { return _vertices[handle.index]; }

    auto& uvPointData(UVPointHandle handle) { return _uvPoints[handle.index]; }
    auto& uvPointData(UVPointHandle handle) const { return _uvPoints[handle.index]; }

    auto& edgeData(EdgeHandle handle) { return _edges[handle.index]; }
    auto& edgeData(EdgeHandle handle) const { return _edges[handle.index]; }

    auto& faceData(FaceHandle handle) { return _faces[handle.index]; }
    auto& faceData(FaceHandle handle) const { return _faces[handle.index]; }

    std::vector<VertexData> _vertices;
    std::vector<UVPointData> _uvPoints;
    std::vector<EdgeData> _edges;
    std::vector<FaceData> _faces;

public:
    VertexHandle addVertex();
    UVPointHandle addUVPoint(VertexHandle v);
    EdgeHandle addEdge(VertexHandle v0, VertexHandle v1);
    FaceHandle addFace(const std::vector<UVPointHandle>& uvPoints);

    void removeVertex(VertexHandle v);
    void removeEdge(EdgeHandle e);
    void removeFace(FaceHandle f);

    void collectGarbage();

    auto& uvPoints(VertexHandle v) const { return  vertexData(v).uvPoints; }
    auto& edges(VertexHandle v) const { return vertexData(v).edges; }

    auto& vertex(UVPointHandle p) const { return uvPointData(p).vertex; }
    auto& faces(UVPointHandle p) const { return uvPointData(p).faces; }

    auto faces(VertexHandle v) const {
        return vertexData(v).uvPoints | ranges::view::transform([this] (UVPointHandle uvPoint) {
            return faces(uvPoint);
        }) | ranges::action::join;
    }

    auto& vertices(EdgeHandle e) const { return _edges[e.index].vertices; }

    auto vertices(FaceHandle f) const {
        return faceData(f).uvPoints | ranges::view::transform([this] (UVPointHandle uvPoint) {
            return vertex(uvPoint);
        });
    }

    auto& uvPoints(FaceHandle f) const { return faceData(f).uvPoints; }
    auto& edges(FaceHandle f) const { return faceData(f).edges; }

    glm::vec3 position(VertexHandle v) const { return vertexData(v).position; }
    void setPosition(VertexHandle v, glm::vec3 pos) { vertexData(v).position = pos; }

    glm::vec2 uv(UVPointHandle uv) const { return uvPointData(uv).position; }
    void setUV(UVPointHandle uv, glm::vec2 pos) { uvPointData(uv).position = pos; }

    bool isSmooth(EdgeHandle edge) { return edgeData(edge).isSmooth; }
    void setIsSmooth(EdgeHandle edge, bool smooth) { edgeData(edge).isSmooth = smooth; }
};

} // namespace NewMesh
} // namespace Lattice
