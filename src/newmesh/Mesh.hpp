#pragma once
#include <vector>
#include <array>
#include <glm/glm.hpp>

namespace Lattice {
namespace NewMesh {

class Vertex {
public:
    bool isDeleted = false;
    glm::vec3 position;
    std::vector<int> uvPoints;
    std::vector<int> edges;
};

class UVPoint {
public:
    glm::vec2 position;
    int vertex;
    std::vector<int> faces;
};

class Edge {
public:
    bool isDeleted = false;
    bool isSmooth = true;
    std::array<int, 2> vertices;
    std::vector<int> faces;
};

class Face {
public:
    bool isDeleted = false;
    std::vector<int> uvPoints;
    std::vector<int> edges;
    int material;
};

class VertexHandle {
public:
    VertexHandle(uint32_t index) : _index(index) {}

    glm::vec3 position() const;
    void setPosition(glm::vec3 position);
    uint32_t index() const;

private:
    uint32_t _index;
};

class UVPointHandle {
public:
    glm::vec2 position() const;
    void setPosition(glm::vec2 position);
    uint32_t index() const;
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

private:
    std::vector<Vertex> _vertices;
    std::vector<UVPoint> _uvPoints;
    std::vector<Edge> _edges;
    std::vector<Face> _faces;
};

} // namespace NewMesh
} // namespace Lattice
