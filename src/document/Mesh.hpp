#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "../support/Hash.hpp"
#include "../support/Pointer.hpp"

namespace Lattice {

class MeshVertex;
class MeshEdge;
class MeshFace;
class Mesh;

class MeshVertex {
public:
    SP<Mesh> mesh() const;
    glm::vec3 position() const;
    void setPosition(glm::vec3 position);

    std::vector<SP<MeshEdge>> edges() const;
    std::vector<SP<MeshFace>> faces() const;

private:
    WP<Mesh> _mesh;
};

class MeshEdge {
public:
    MeshEdge(const std::pair<SP<MeshVertex>, SP<MeshVertex>>& vertices, const SP<Mesh>& mesh) : _vertices(vertices), _mesh(mesh) {}

    auto mesh() const { return _mesh.lock(); }
    auto& vertices() const { return _vertices; }
    std::vector<SP<MeshFace>> faces() const;

private:
    std::pair<SP<MeshVertex>, SP<MeshVertex>> _vertices;
    WP<Mesh> _mesh;
};

class MeshFace {
public:
    MeshFace(const std::vector<SP<MeshVertex>>& vertices, const SP<Mesh>& mesh) : _vertices(vertices), _mesh(mesh) {}

    auto mesh() const { return _mesh.lock(); }
    auto& vertices() const { return _vertices; }
    std::vector<SP<MeshEdge>> edges() const;

private:
    std::vector<SP<MeshVertex>> _vertices;
    WP<Mesh> _mesh;
};

class Mesh : public std::enable_shared_from_this<Mesh> {
public:
    Mesh();

    SP<MeshVertex> addVertex();
    SP<MeshEdge> addEdge(const std::pair<SP<MeshVertex>, SP<MeshVertex>>& vertices);
    SP<MeshFace> addFace(const std::vector<SP<MeshVertex>>& vertices);

    const auto& vertices() const { return _vertices; }
    const auto& edges() const { return _edges; }
    const auto& faces() const { return _faces; }

    std::vector<SP<MeshEdge>> edgesForVertex(const SP<MeshVertex>& vertex) const;
    std::vector<SP<MeshFace>> facesForVertex(const SP<MeshVertex>& vertex) const;
    std::vector<SP<MeshEdge>> edgesForFace(const SP<MeshFace>& face) const;
    std::vector<SP<MeshFace>> facesForEdge(const SP<MeshEdge>& edge) const;

private:

    std::unordered_set<SP<MeshVertex>> _vertices;
    std::unordered_map<std::pair<SP<MeshVertex>, SP<MeshVertex>>, SP<MeshEdge>> _edges;
    std::unordered_map<std::vector<SP<MeshVertex>>, SP<MeshFace>> _faces;
    std::unordered_multimap<SP<MeshVertex>, SP<MeshEdge>> _edgesForVertex;
    std::unordered_multimap<SP<MeshVertex>, SP<MeshFace>> _facesForVertex;
    std::unordered_multimap<SP<MeshFace>, SP<MeshEdge>> _edgesForFace;
    std::unordered_multimap<SP<MeshEdge>, SP<MeshFace>> _facesForEdge;
};

} // namespace Lattice
