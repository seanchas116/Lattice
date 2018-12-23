#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "../support/Hash.hpp"
#include "../support/Pointer.hpp"

namespace Lattice {

class Vertex;
class Edge;
class Face;
class Mesh;

class Vertex {
public:
    SP<Mesh> mesh() const;
    glm::vec3 position() const;
    void setPosition(glm::vec3 position);

    std::vector<SP<Edge>> edges() const;
    std::vector<SP<Face>> faces() const;

private:
    WP<Mesh> _mesh;
};

class Edge {
public:
    Edge(const std::pair<SP<Vertex>, SP<Vertex>>& vertices, const SP<Mesh>& mesh) : _vertices(vertices), _mesh(mesh) {}

    auto mesh() const { return _mesh.lock(); }
    auto& vertices() const { return _vertices; }
    std::vector<SP<Face>> faces() const;

private:
    std::pair<SP<Vertex>, SP<Vertex>> _vertices;
    WP<Mesh> _mesh;
};

class Face {
public:
    Face(const std::vector<SP<Vertex>>& vertices, const SP<Mesh>& mesh) : _vertices(vertices), _mesh(mesh) {}

    auto mesh() const { return _mesh.lock(); }
    auto& vertices() const { return _vertices; }
    std::vector<SP<Edge>> edges() const;

private:
    std::vector<SP<Vertex>> _vertices;
    WP<Mesh> _mesh;
};

class Mesh : public std::enable_shared_from_this<Mesh> {
public:
    Mesh();

    SP<Vertex> addVertex();
    SP<Edge> addEdge(const std::pair<SP<Vertex>, SP<Vertex>>& vertices);
    SP<Face> addFace(const std::vector<SP<Vertex>>& vertices);

    const auto& vertices() const { return _vertices; }
    const auto& edges() const { return _edges; }
    const auto& faces() const { return _faces; }

    std::vector<SP<Edge>> edgesForVertex(const SP<Vertex>& vertex) const;
    std::vector<SP<Face>> facesForVertex(const SP<Vertex>& vertex) const;
    std::vector<SP<Edge>> edgesForFace(const SP<Face>& face) const;
    std::vector<SP<Face>> facesForEdge(const SP<Edge>& edge) const;

private:

    std::unordered_set<SP<Vertex>> _vertices;
    std::unordered_map<std::pair<SP<Vertex>, SP<Vertex>>, SP<Edge>> _edges;
    std::unordered_map<std::vector<SP<Vertex>>, SP<Face>> _faces;
    std::unordered_multimap<SP<Vertex>, SP<Edge>> _edgesForVertex;
    std::unordered_multimap<SP<Vertex>, SP<Face>> _facesForVertex;
    std::unordered_multimap<SP<Face>, SP<Edge>> _edgesForFace;
    std::unordered_multimap<SP<Edge>, SP<Face>> _facesForEdge;
};

} // namespace Lattice
