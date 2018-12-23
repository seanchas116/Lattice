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

namespace {

template <typename K, typename V>
std::vector<V> values(const std::unordered_multimap<K, V>& multimap, const K& key) {
    auto range = multimap.equal_range(key);
    std::vector<V> result;
    for (auto it = range.first; it != range.second; ++it) {
        result.push_back(it->second);
    }
    return result;
}

}

std::vector<SP<Edge> > Mesh::edgesForVertex(const SP<Vertex> &vertex) const {
    return values(_edgesForVertex, vertex);
}

std::vector<SP<Face> > Mesh::facesForVertex(const SP<Vertex> &vertex) const {
    return values(_facesForVertex, vertex);
}

std::vector<SP<Edge> > Mesh::edgesForFace(const SP<Face> &face) const {
    return values(_edgesForFace, face);
}

std::vector<SP<Face> > Mesh::facesForEdge(const SP<Edge> &edge) const {
    return values(_facesForEdge, edge);
}

} // namespace Lattice
