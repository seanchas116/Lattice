#include "FindConnected.hpp"

namespace Lattice {
namespace Mesh {

namespace {

void addConnectedRecursive(std::unordered_set<VertexHandle>& connectedVertices, const Mesh& mesh, VertexHandle vertex) {
    if (connectedVertices.find(vertex) != connectedVertices.end()) {
        return;
    }
    connectedVertices.insert(vertex);

    for (auto&& edge : mesh.edges(vertex)) {
        for (auto&& v : mesh.vertices(edge)) {
            if (v != vertex) {
                addConnectedRecursive(connectedVertices, mesh, v);
            }
        }
    }
}

}

std::unordered_set<VertexHandle> findConnected(const Mesh &mesh, const std::vector<VertexHandle> &vertices) {
    std::unordered_set<VertexHandle> connectedVertices;

    for (auto&& v : vertices) {
        addConnectedRecursive(connectedVertices, mesh, v);
    }

    return connectedVertices;
}

} // namespace Mesh
} // namespace Lattice
