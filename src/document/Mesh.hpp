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
    glm::vec3 position() const { return _position; }
    void setPosition(glm::vec3 position) { _position = position; }

    std::vector<SP<MeshEdge>> edges() const;
    std::vector<SP<MeshFace>> faces() const;

private:
    friend class MeshEdge;
    friend class MeshFace;
    glm::vec3 _position;
    std::unordered_set<MeshEdge*> _edges;
    std::unordered_set<MeshFace*> _faces;
};

class MeshEdge : public std::enable_shared_from_this<MeshEdge> {
public:
    MeshEdge(const std::pair<SP<MeshVertex>, SP<MeshVertex>>& vertices);
    ~MeshEdge();

    auto& vertices() const { return _vertices; }
    std::vector<SP<MeshFace>> faces() const;

private:
    friend class MeshFace;
    std::pair<SP<MeshVertex>, SP<MeshVertex>> _vertices;
    std::unordered_set<MeshFace*> _faces;
};

class MeshFace : public std::enable_shared_from_this<MeshFace> {
public:
    MeshFace(const std::vector<SP<MeshVertex>>& vertices, const std::vector<SP<MeshEdge>>& edges);
    ~MeshFace();

    auto& vertices() const { return _vertices; }
    auto& edges() const { return _edges; }

private:
    WP<Mesh> _mesh;
    std::vector<SP<MeshVertex>> _vertices;
    std::vector<SP<MeshEdge>> _edges;
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

private:

    std::unordered_set<SP<MeshVertex>> _vertices;
    std::unordered_map<std::pair<SP<MeshVertex>, SP<MeshVertex>>, SP<MeshEdge>> _edges;
    std::unordered_map<std::vector<SP<MeshVertex>>, SP<MeshFace>> _faces;
};

} // namespace Lattice
