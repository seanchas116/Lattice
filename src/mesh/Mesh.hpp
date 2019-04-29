#pragma once
#include <vector>
#include <array>
#include <glm/glm.hpp>
#include <range/v3/all.hpp>

namespace Lattice {
namespace Mesh {

class Mesh;

template <typename TDerived>
struct Handle {
    explicit Handle(uint32_t index) : index(index) {}
    bool operator==(const TDerived& other) const { return index == other.index; }
    bool operator!=(const TDerived& other) const { return !operator==(other); }
    uint32_t index;
};

struct VertexHandle : public Handle<VertexHandle> {
    VertexHandle() : Handle(0) {}
    explicit VertexHandle(uint32_t index) : Handle(index) {}
};

struct UVPointHandle : public Handle<UVPointHandle> {
    UVPointHandle() : Handle(0) {}
    explicit UVPointHandle(uint32_t index) : Handle(index) {}
};

struct EdgeHandle : public Handle<EdgeHandle> {
    EdgeHandle() : Handle(0) {}
    EdgeHandle(uint32_t index) : Handle(index) {}
};

struct FaceHandle : public Handle<FaceHandle> {
    FaceHandle() : Handle(0) {}
    FaceHandle(uint32_t index) : Handle(index) {}
};


struct VertexData {
    bool isDeleted = false;
    glm::vec3 position = glm::vec3(0);
    std::vector<UVPointHandle> uvPoints;
    std::vector<EdgeHandle> edges;
};

struct UVPointData {
    bool isDeleted = false;
    glm::vec2 position = glm::vec2(0);
    VertexHandle vertex;
    std::vector<FaceHandle> faces;
};

struct EdgeData {
    bool isDeleted = false;
    bool isSmooth = true;
    std::array<VertexHandle, 2> vertices;
    std::vector<FaceHandle> faces;
};

struct FaceData {
    bool isDeleted = false;
    uint32_t material = 0;
    std::vector<UVPointHandle> uvPoints;
    std::vector<EdgeHandle> edges;
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
    VertexHandle addVertex(glm::vec3 position);
    UVPointHandle addUVPoint(VertexHandle v, glm::vec2 position);
    EdgeHandle addEdge(VertexHandle v0, VertexHandle v1, bool isSmooth);
    FaceHandle addFace(const std::vector<UVPointHandle>& uvPoints, uint32_t material);

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

    uint32_t material(FaceHandle face) { return faceData(face).material; }
    void setMaterial(FaceHandle face, uint32_t material) { faceData(face).material = material; }
};

} // namespace NewMesh
} // namespace Lattice
