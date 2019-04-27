#pragma once
#include <vector>
#include <array>
#include <glm/glm.hpp>
#include <range/v3/all.hpp>

namespace Lattice {
namespace NewMesh {

class Mesh;

class Vertex {
public:
    bool isDeleted = false;
    glm::vec3 position;
    std::vector<uint32_t> uvPoints;
    std::vector<uint32_t> edges;
};

class UVPoint {
public:
    glm::vec2 position;
    uint32_t vertex;
    std::vector<uint32_t> faces;
};

class Edge {
public:
    bool isDeleted = false;
    bool isSmooth = true;
    std::array<uint32_t, 2> vertices;
    std::vector<uint32_t> faces;
};

class Face {
public:
    bool isDeleted = false;
    std::vector<uint32_t> uvPoints;
    std::vector<uint32_t> edges;
    int material;
};

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
    explicit VertexHandle(uint32_t index) : Handle(index) {}
};

class UVPointHandle : public Handle<UVPointHandle> {
public:
    explicit UVPointHandle(uint32_t index) : Handle(index) {}
};

class EdgeHandle : public Handle<EdgeHandle> {
public:
    EdgeHandle(uint32_t index) : Handle(index) {}
};

class FaceHandle : public Handle<FaceHandle> {
public:
    FaceHandle(uint32_t index) : Handle(index) {}
};

class Mesh {
public:
    void collectGarbage();

    VertexHandle addVertex();
    UVPointHandle addUVPoint(VertexHandle v);
    EdgeHandle addEdge(VertexHandle v0, VertexHandle v1);
    FaceHandle addFace(const std::vector<UVPointHandle>& uvPoints);

    auto uvPoints(VertexHandle v) const {
        return  _vertices[v.index].uvPoints | ranges::view::transform([] (uint32_t index) {
            return UVPointHandle(index);
        });
    }

    auto edges(VertexHandle v) const {
        return _vertices[v.index].edges | ranges::view::transform([] (uint32_t index) {
            return EdgeHandle(index);
        });
    }

    auto faces(UVPointHandle p) const {
        return _uvPoints[p.index].faces | ranges::view::transform([] (uint32_t index) {
            return FaceHandle(index);
        });
    }

    auto faces(VertexHandle v) const {
        return _vertices[v.index].uvPoints | ranges::view::transform([this] (uint32_t index) {
            return faces(UVPointHandle(index));
        }) | ranges::action::join;
    }

    auto vertices(FaceHandle f) const {
        return _faces[f.index].uvPoints | ranges::view::transform([this] (uint32_t index) {
            return VertexHandle(_uvPoints[index].vertex);
        });
    }

    auto uvPoints(FaceHandle f) const {
        return _faces[f.index].uvPoints | ranges::view::transform([] (uint32_t index) {
            return UVPointHandle(index);
        });
    }

    auto edges(FaceHandle f) const {
        return _faces[f.index].edges | ranges::view::transform([] (uint32_t index) {
            return EdgeHandle(index);
        });
    }

    std::array<VertexHandle, 2> vertices(EdgeHandle e) const {
        auto& edge = _edges[e.index];
        return {VertexHandle(edge.vertices[0]), VertexHandle(edge.vertices[1])};
    }

    glm::vec3 position(VertexHandle v) const {
        return _vertices[v.index].position;
    }

    void setPosition(VertexHandle v, glm::vec3 pos) {
        _vertices[v.index].position = pos;
    }

    glm::vec2 uv(UVPointHandle uv) const {
        return _uvPoints[uv.index].position;
    }

    void setUV(UVPointHandle uv, glm::vec2 pos) {
        _uvPoints[uv.index].position = pos;
    }

    bool isSmooth(EdgeHandle edge) {
        return _edges[edge.index].isSmooth;
    }

    void setIsSmooth(EdgeHandle edge, bool smooth) {
       _edges[edge.index].isSmooth = smooth;
    }

private:
    friend class VertexHandle;

    std::vector<Vertex> _vertices;
    std::vector<UVPoint> _uvPoints;
    std::vector<Edge> _edges;
    std::vector<Face> _faces;
};

} // namespace NewMesh
} // namespace Lattice
