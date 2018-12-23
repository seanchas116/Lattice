#include "Mesh.hpp"

namespace Lattice {

std::vector<SP<MeshEdge> > MeshVertex::edges() const {
    return mesh()->edgesForVertex(shared_from_this());
}

std::vector<SP<MeshFace> > MeshVertex::faces() const {
    return mesh()->facesForVertex(shared_from_this());
}

std::vector<SP<MeshFace> > MeshEdge::faces() const {
    return mesh()->facesForEdge(shared_from_this());
}

Mesh::Mesh() {
}

SP<MeshVertex> Mesh::addVertex() {
    auto vertex = std::make_shared<MeshVertex>();
    _vertices.insert(vertex);
    return vertex;
}

SP<MeshEdge> Mesh::addEdge(const std::pair<SP<MeshVertex>, SP<MeshVertex> > &vertices) {
    auto edge = std::make_shared<MeshEdge>(shared_from_this(), vertices);
    _edges[vertices] = edge;
    _edgesForVertex.insert({vertices.first, edge});
    _edgesForVertex.insert({vertices.second, edge});
    return edge;
}

SP<MeshFace> Mesh::addFace(const std::vector<SP<MeshVertex> > &vertices) {
    std::vector<SP<MeshEdge>> edges;
    for (size_t i = 0; i < vertices.size(); ++i) {
        edges.push_back(addEdge({vertices[i], vertices[(i + 1) % vertices.size()]}));
    }

    auto face = std::make_shared<MeshFace>(shared_from_this(), vertices, edges);
    _faces[vertices] = face;

    for (auto& v : vertices) {
        _facesForVertex.insert({v, face});
    }
    for (auto& e : edges) {
        _facesForEdge.insert({e, face});
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

std::vector<SP<MeshEdge> > Mesh::edgesForVertex(const SP<const MeshVertex> &vertex) const {
    return values(_edgesForVertex, vertex);
}

std::vector<SP<MeshFace> > Mesh::facesForVertex(const SP<const MeshVertex> &vertex) const {
    return values(_facesForVertex, vertex);
}

std::vector<SP<MeshFace> > Mesh::facesForEdge(const SP<const MeshEdge> &edge) const {
    return values(_facesForEdge, edge);
}

} // namespace Lattice
