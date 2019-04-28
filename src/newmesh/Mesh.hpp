#pragma once
#include <vector>
#include <array>
#include <glm/glm.hpp>
#include <range/v3/all.hpp>

namespace Lattice {
namespace NewMesh {

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


class Vertex {
public:
    bool isDeleted = false;
    glm::vec3 position;
    std::vector<UVPointHandle> uvPoints;
    std::vector<EdgeHandle> edges;
};

class UVPoint {
public:
    glm::vec2 position;
    VertexHandle vertex;
    std::vector<FaceHandle> faces;
};

class Edge {
public:
    bool isDeleted = false;
    bool isSmooth = true;
    std::array<VertexHandle, 2> vertices;
    std::vector<FaceHandle> faces;
};

class Face {
public:
    bool isDeleted = false;
    std::vector<UVPointHandle> uvPoints;
    std::vector<EdgeHandle> edges;
    int material;
};


class Mesh {
public:
    void collectGarbage();

    VertexHandle addVertex();
    UVPointHandle addUVPoint(VertexHandle v);
    EdgeHandle addEdge(VertexHandle v0, VertexHandle v1);
    FaceHandle addFace(const std::vector<UVPointHandle>& uvPoints);

    auto uvPoints(VertexHandle v) const { return  _vertices[v.index].uvPoints; }
    auto edges(VertexHandle v) const { return _vertices[v.index].edges; }

    auto vertex(UVPointHandle p) const { return _uvPoints[p.index].vertex; }
    auto faces(UVPointHandle p) const { return _uvPoints[p.index].faces; }

    auto faces(VertexHandle v) const {
        return _vertices[v.index].uvPoints | ranges::view::transform([this] (UVPointHandle uvPoint) {
            return faces(uvPoint);
        }) | ranges::action::join;
    }

    std::array<VertexHandle, 2> vertices(EdgeHandle e) const { return _edges[e.index].vertices; }

    auto vertices(FaceHandle f) const {
        return _faces[f.index].uvPoints | ranges::view::transform([this] (UVPointHandle uvPoint) {
            return _uvPoints[uvPoint.index].vertex;
        });
    }

    auto uvPoints(FaceHandle f) const { return _faces[f.index].uvPoints; }
    auto edges(FaceHandle f) const { return _faces[f.index].edges; }

    glm::vec3 position(VertexHandle v) const { return _vertices[v.index].position; }
    void setPosition(VertexHandle v, glm::vec3 pos) { _vertices[v.index].position = pos; }

    glm::vec2 uv(UVPointHandle uv) const { return _uvPoints[uv.index].position; }
    void setUV(UVPointHandle uv, glm::vec2 pos) { _uvPoints[uv.index].position = pos; }

    bool isSmooth(EdgeHandle edge) { return _edges[edge.index].isSmooth; }
    void setIsSmooth(EdgeHandle edge, bool smooth) { _edges[edge.index].isSmooth = smooth; }

private:
    friend class VertexHandle;

    std::vector<Vertex> _vertices;
    std::vector<UVPoint> _uvPoints;
    std::vector<Edge> _edges;
    std::vector<Face> _faces;
};

} // namespace NewMesh
} // namespace Lattice
