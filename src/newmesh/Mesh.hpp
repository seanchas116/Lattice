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

template <typename T>
class Handle {
};

class Mesh {
public:
    Mesh();

    void collectGarbage();

    Handle<Vertex> addVertex();
    Handle<UVPoint> addUVPoint(Handle<Vertex> v);
    Handle<Edge> addEdge(Handle<Vertex> v0, Handle<Vertex> v1);
    Handle<Face> addFace(const std::vector<Handle<UVPoint>>& uvPoints);

    std::vector<Vertex> vertices;
    std::vector<UVPoint> uvPoints;
    std::vector<Edge> edges;
    std::vector<Face> faces;
};

} // namespace NewMesh
} // namespace Lattice
