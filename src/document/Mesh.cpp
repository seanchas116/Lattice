#include "Mesh.hpp"
#include "../support/Mod.hpp"

namespace Lattice {

std::vector<SP<MeshEdge> > MeshVertex::edges() const {
    std::vector<SP<MeshEdge>> edges;
    for (auto& e : _edges) {
        edges.push_back(e->shared_from_this());
    }
    return edges;
}

std::vector<SP<MeshFace> > MeshVertex::faces() const {
    std::vector<SP<MeshFace>> faces;
    for (auto& f : _faces) {
        faces.push_back(f->shared_from_this());
    }
    return faces;
}

MeshEdge::MeshEdge(const std::array<SP<MeshVertex>, 2> &vertices) : _vertices(vertices) {
    vertices[0]->_edges.insert(this);
    vertices[1]->_edges.insert(this);
}

MeshEdge::~MeshEdge() {
    _vertices[0]->_edges.erase(this);
    _vertices[1]->_edges.erase(this);
}

std::vector<SP<MeshFace> > MeshEdge::faces() const {
    std::vector<SP<MeshFace>> faces;
    for (auto& f : _faces) {
        faces.push_back(f->shared_from_this());
    }
    return faces;
}

MeshFace::MeshFace(const std::vector<SP<MeshVertex> > &vertices, const std::vector<SP<MeshEdge> > &edges) : _vertices(vertices), _edges(edges) {
    for (auto& v : vertices) {
        v->_faces.insert(this);
    }
    for (auto& e : edges) {
        e->_faces.insert(this);
    }
}

MeshFace::~MeshFace() {
    for (auto& v : _vertices) {
        v->_faces.erase(this);
    }
    for (auto& e : _edges) {
        e->_faces.erase(this);
    }
}

glm::vec3 MeshFace::normal() const {
    if (_vertices.size() == 3) {
        return normalize(cross(_vertices[1]->position() - _vertices[0]->position(), _vertices[2]->position() - _vertices[0]->position()));
    }

    // find average vertex normal
    glm::vec3 normalSum(0);
    size_t count = _vertices.size();

    for (size_t i = 0; i < count; ++i) {
        auto prev = _vertices[mod(i - 1, count)]->position();
        auto curr = _vertices[i]->position();
        auto next = _vertices[mod(i + 1, count)]->position();
        auto normal = normalize(cross(next - curr, prev - curr));
        normalSum += normal;
    }

    return normalSum / float(count);
}

Mesh::Mesh() {
}

SP<MeshVertex> Mesh::addVertex(glm::vec3 position) {
    auto vertex = std::make_shared<MeshVertex>();
    vertex->setPosition(position);
    _vertices.insert(vertex);
    return vertex;
}

SP<MeshEdge> Mesh::addEdge(const std::array<SP<MeshVertex>, 2> &vertices) {
    auto it = _edges.find(vertices);
    if (it != _edges.end()) {
        return it->second;
    }

    auto edge = std::make_shared<MeshEdge>(vertices);
    _edges[vertices] = edge;
    return edge;
}

SP<MeshFace> Mesh::addFace(const std::vector<SP<MeshVertex> > &vertices) {
    auto it = _faces.find(vertices);
    if (it != _faces.end()) {
        return it->second;
    }

    std::vector<SP<MeshEdge>> edges;
    for (size_t i = 0; i < vertices.size(); ++i) {
        edges.push_back(addEdge({vertices[i], vertices[(i + 1) % vertices.size()]}));
    }

    auto face = std::make_shared<MeshFace>(vertices, edges);
    _faces[vertices] = face;
    return face;
}

SP<Mesh> Mesh::clone() const {
    throw std::runtime_error("not implemented");
}

} // namespace Lattice
