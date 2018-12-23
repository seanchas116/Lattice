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

class MeshVertex : public std::enable_shared_from_this<MeshVertex> {
public:
    SP<Mesh> mesh() const { return _mesh.lock(); }
    glm::vec3 position() const { return _position; }
    void setPosition(glm::vec3 position) { _position = position; }

    std::vector<SP<MeshEdge>> edges() const;
    std::vector<SP<MeshFace>> faces() const;

private:
    WP<Mesh> _mesh;
    glm::vec3 _position;
};

class MeshEdge : public std::enable_shared_from_this<MeshEdge> {
public:
    MeshEdge(const SP<Mesh>& mesh, const std::pair<SP<MeshVertex>, SP<MeshVertex>>& vertices) : _mesh(mesh), _vertices(vertices) {}

    auto mesh() const { return _mesh.lock(); }
    auto& vertices() const { return _vertices; }
    std::vector<SP<MeshFace>> faces() const;

private:
    WP<Mesh> _mesh;
    std::pair<SP<MeshVertex>, SP<MeshVertex>> _vertices;
};

class MeshFace : public std::enable_shared_from_this<MeshFace> {
public:
    MeshFace(const SP<Mesh>& mesh, const std::vector<SP<MeshVertex>>& vertices) : _mesh(mesh), _vertices(vertices) {}

    auto mesh() const { return _mesh.lock(); }
    auto& vertices() const { return _vertices; }
    std::vector<SP<MeshEdge>> edges() const;

private:
    WP<Mesh> _mesh;
    std::vector<SP<MeshVertex>> _vertices;
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

    std::vector<SP<MeshEdge>> edgesForVertex(const SP<const MeshVertex>& vertex) const;
    std::vector<SP<MeshFace>> facesForVertex(const SP<const MeshVertex>& vertex) const;
    std::vector<SP<MeshEdge>> edgesForFace(const SP<const MeshFace>& face) const;
    std::vector<SP<MeshFace>> facesForEdge(const SP<const MeshEdge>& edge) const;

private:

    std::unordered_set<SP<MeshVertex>> _vertices;
    std::unordered_map<std::pair<SP<MeshVertex>, SP<MeshVertex>>, SP<MeshEdge>> _edges;
    std::unordered_map<std::vector<SP<MeshVertex>>, SP<MeshFace>> _faces;
    std::unordered_multimap<SP<const MeshVertex>, SP<MeshEdge>> _edgesForVertex;
    std::unordered_multimap<SP<const MeshVertex>, SP<MeshFace>> _facesForVertex;
    std::unordered_multimap<SP<const MeshFace>, SP<MeshEdge>> _edgesForFace;
    std::unordered_multimap<SP<const MeshEdge>, SP<MeshFace>> _facesForEdge;
};

} // namespace Lattice
