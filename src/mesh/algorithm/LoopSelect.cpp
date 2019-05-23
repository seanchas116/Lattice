#include "LoopSelect.hpp"
#include <range/v3/algorithm/find.hpp>

namespace Lattice {
namespace Mesh {

void LoopSelect::perform(Mesh &mesh) const {
    std::vector<EdgeHandle> edges;

    auto edge = this->edge;
    auto vertex = mesh.vertices(edge)[0];
    edges.push_back(edge);

    auto edgeFaces = mesh.faces(edge) | ranges::to_vector;
    if (edgeFaces.size() != 2) {
        return;
    }

    while (true) {
        auto nextVertex = mesh.vertices(edge)[0] == vertex ? mesh.vertices(edge)[1] : mesh.vertices(edge)[0];
        if (ranges::distance(mesh.faces(nextVertex)) != 4) {
            // extraordinally vertex
            return;
        }

        EdgeHandle nextEdge;
        bool nextEdgeFound = false;
        std::vector<FaceHandle> nextEdgeFaces;

        for (auto e : mesh.edges(nextVertex)) {
            auto faces = mesh.faces(e) | ranges::to_vector;
            if (faces.size() != 2) {
                // non-manifold edge
                continue;
            }

            bool allFacesDifferent = true;
            for (auto face : faces) {
                for (auto edgeFace : edgeFaces) {
                    if (edgeFace == face) {
                        allFacesDifferent = false;
                    }
                }
            }
            if (allFacesDifferent) {
                nextEdge = e;
                nextEdgeFaces = std::move(edgeFaces);
                nextEdgeFound = true;
                break;
            }
        }

        if (!nextEdgeFound) {
            return;
        }

        if (nextEdge == edges[0]) {
            // loop found

            for (auto e : edges) {
                for (auto v : mesh.vertices(e)) {
                    mesh.setSelected(v, true);
                }
            }

            return;
        }

        if (ranges::find(edges, nextEdge) != edges.end()) {
            // 9 loop
            return;
        }

        edges.push_back(nextEdge);
        edge = nextEdge;
        edgeFaces = std::move(nextEdgeFaces);
    }
}

} // namespace Mesh
} // namespace Lattice
