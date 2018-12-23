#include "Mesh.hpp"

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

MeshEdge::MeshEdge(const std::pair<SP<MeshVertex>, SP<MeshVertex> > &vertices) : _vertices(vertices) {
    vertices.first->_edges.insert(this);
    vertices.second->_edges.insert(this);
}

MeshEdge::~MeshEdge() {
    _vertices.first->_edges.erase(this);
    _vertices.second->_edges.erase(this);
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

Mesh::Mesh() {
}

SP<MeshVertex> Mesh::addVertex() {
    auto vertex = std::make_shared<MeshVertex>();
    _vertices.insert(vertex);
    return vertex;
}

SP<MeshEdge> Mesh::addEdge(const std::pair<SP<MeshVertex>, SP<MeshVertex> > &vertices) {
    auto edge = std::make_shared<MeshEdge>(vertices);
    _edges[vertices] = edge;
    return edge;
}

SP<MeshFace> Mesh::addFace(const std::vector<SP<MeshVertex> > &vertices) {
    std::vector<SP<MeshEdge>> edges;
    for (size_t i = 0; i < vertices.size(); ++i) {
        edges.push_back(addEdge({vertices[i], vertices[(i + 1) % vertices.size()]}));
    }

    auto face = std::make_shared<MeshFace>(vertices, edges);
    _faces[vertices] = face;
    return face;
}

} // namespace Lattice
