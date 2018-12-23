#include "Mesh.hpp"

namespace Lattice {

Mesh::Mesh() {
}

SP<Vertex> Mesh::addVertex() {
    auto vertex = std::make_shared<Vertex>();
    _vertices.insert(vertex);
    return vertex;
}

SP<Edge> Mesh::addEdge(const std::pair<SP<Vertex>, SP<Vertex> > &vertices) {
    auto edge = std::make_shared<Edge>(vertices, shared_from_this());
    _edges[vertices] = edge;
    _edgesForVertex.insert({vertices.first, edge});
    _edgesForVertex.insert({vertices.second, edge});
    return edge;
}

SP<Face> Mesh::addFace(const std::vector<SP<Vertex> > &vertices) {
    auto face = std::make_shared<Face>(vertices, shared_from_this());
    _faces[vertices] = face;
    for (auto& v : vertices) {
        _facesForVertex.insert({v, face});
    }
    return face;
}

std::vector<SP<Edge> > Mesh::edgesForVertex(const SP<Vertex> &vertex) const {
    auto range = _edgesForVertex.equal_range(vertex);
    std::vector<SP<Edge>> edges;
    for (auto it = range.first; it != range.second; ++it) {
        edges.push_back(it->second);
    }
    return edges;
}

std::vector<SP<Face> > Mesh::facesForVertex(const SP<Vertex> &vertex) const {
    auto range = _facesForVertex.equal_range(vertex);
    std::vector<SP<Face>> faces;
    for (auto it = range.first; it != range.second; ++it) {
        faces.push_back(it->second);
    }
    return faces;
}

} // namespace Lattice
