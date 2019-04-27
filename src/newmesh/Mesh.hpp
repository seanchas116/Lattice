#pragma once
#include <vector>
#include <array>
#include <glm/glm.hpp>
#include <boost/range.hpp>
#include <boost/range/adaptor/transformed.hpp>

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

class VertexHandle {
public:
    VertexHandle(uint32_t index) : _index(index) {}
    uint32_t index() const;
private:
    uint32_t _index;
};

class UVPointHandle {
public:
    UVPointHandle(uint32_t index) :  _index(index) {}
    uint32_t index() const;
private:
    uint32_t _index;
};

class EdgeHandle {
public:
    bool isSmooth() const;
    void setSmooth(bool smooth);
    uint32_t index() const;
};

class FaceHandle {
public:
};

class MaterialHandle {
public:
};

class Mesh {
public:
    Mesh();

    void collectGarbage();

    VertexHandle addVertex();
    UVPointHandle addUVPoint(VertexHandle v);
    EdgeHandle addEdge(VertexHandle v0, VertexHandle v1);
    FaceHandle addFace(const std::vector<UVPointHandle>& uvPoints);

    auto uvPoints(VertexHandle v) const {
        return boost::adaptors::transform(_vertices[v.index()].uvPoints, [] (uint32_t index) {
            return UVPointHandle(index);
        });
    }

    glm::vec3 position(VertexHandle v) const {
        return _vertices[v.index()].position;
    }

    void setPosition(VertexHandle v, glm::vec3 pos) {
        _vertices[v.index()].position = pos;
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
