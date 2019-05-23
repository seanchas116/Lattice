#include "LoopSelect.hpp"
#include <range/v3/algorithm/find.hpp>

namespace Lattice {
namespace Mesh {

void LoopSelect::perform(Mesh &mesh) const {
    std::vector<EdgeHandle> edges;

    auto edge = this->edge;
    auto vertex = mesh.vertices(edge)[0];
    edges.push_back(edge);

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
            bool allFacesDifferent = true;
            for (auto face : mesh.faces(e)) {
                for (auto edgeFace : mesh.faces(edge)) {
                    if (edgeFace == face) {
                        allFacesDifferent = false;
                    }
                }
            }
            if (allFacesDifferent) {
                nextEdge = e;
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
    }
}

} // namespace Mesh
} // namespace Lattice
