#include "LoopSelect.hpp"

namespace Lattice {
namespace Mesh {

void LoopSelect::perform(Mesh &mesh) const {
    std::vector<EdgeHandle> edges;

    auto edge = this->edge;
    auto lastVertex = mesh.vertices(edge)[0];
    edges.push_back(edge);

    while (true) {
        auto vertex = mesh.vertices(edge)[0] == lastVertex ? mesh.vertices(edge)[1] : mesh.vertices(edge)[0];
        auto vertexFaces = mesh.faces(vertex) | ranges::to_vector;
        if (vertexFaces.size() != 4) {
            // extraordinally vertex
            return;
        }
    }
}

} // namespace Mesh
} // namespace Lattice
