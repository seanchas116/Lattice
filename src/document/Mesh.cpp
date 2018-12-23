#include "Mesh.hpp"

namespace Lattice {

Mesh::Mesh() {
}

SP<MeshVertex> Mesh::addVertex() {
    auto vertex = std::make_shared<MeshVertex>();
    _vertices.insert(vertex);
    return vertex;
}

SP<MeshEdge> Mesh::addEdge(const std::pair<SP<MeshVertex>, SP<MeshVertex> > &vertices) {
    auto edge = std::make_shared<MeshEdge>(vertices, shared_from_this());
    _edges[vertices] = edge;
    _edgesForVertex.insert({vertices.first, edge});
    _edgesForVertex.insert({vertices.second, edge});
    return edge;
}

SP<MeshFace> Mesh::addFace(const std::vector<SP<MeshVertex> > &vertices) {
    auto face = std::make_shared<MeshFace>(vertices, shared_from_this());
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

std::vector<SP<MeshEdge> > Mesh::edgesForVertex(const SP<MeshVertex> &vertex) const {
    return values(_edgesForVertex, vertex);
}

std::vector<SP<MeshFace> > Mesh::facesForVertex(const SP<MeshVertex> &vertex) const {
    return values(_facesForVertex, vertex);
}

std::vector<SP<MeshEdge> > Mesh::edgesForFace(const SP<MeshFace> &face) const {
    return values(_edgesForFace, face);
}

std::vector<SP<MeshFace> > Mesh::facesForEdge(const SP<MeshEdge> &edge) const {
    return values(_facesForEdge, edge);
}

} // namespace Lattice
