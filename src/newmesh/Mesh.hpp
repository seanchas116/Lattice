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
    glm::vec3 position() const;
    void setPosition(glm::vec3 position);
    uint32_t index() const;
};

class UVPointHandle {
public:
    glm::vec2 position() const;
    void setPosition(glm::vec2 position);
    uint32_t index() const;
};

class EdgeHandle {
};

class FaceHandle {
};

class Mesh {
public:
    Mesh();

    void collectGarbage();

    VertexHandle addVertex();
    UVPointHandle addUVPoint(VertexHandle v);
    EdgeHandle addEdge(VertexHandle v0, VertexHandle v1);
    FaceHandle addFace(const std::vector<UVPointHandle>& uvPoints);

    std::vector<Vertex> vertices;
    std::vector<UVPoint> uvPoints;
    std::vector<Edge> edges;
    std::vector<Face> faces;
};

} // namespace NewMesh
} // namespace Lattice
