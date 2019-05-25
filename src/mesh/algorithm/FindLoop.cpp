#include "FindLoop.hpp"
#include <range/v3/algorithm/find.hpp>
#include <QtDebug>

namespace Lattice {
namespace Mesh {

std::vector<EdgeHandle> FindLoop::perform(const Mesh &mesh) const {
    std::vector<EdgeHandle> edges;

    auto edge = this->edge;
    auto vertex = mesh.vertices(edge)[0];
    edges.push_back(edge);

    if (ranges::distance(mesh.faces(edge)) != 2) {
        // non-manifold edge
        return {};
    }

    while (true) {
        auto nextVertex = mesh.vertices(edge)[0] == vertex ? mesh.vertices(edge)[1] : mesh.vertices(edge)[0];

        if (ranges::distance(mesh.faces(nextVertex)) != 4) {
            // extraordinary vertex
            return {};
        }

        EdgeHandle nextEdge;
        bool nextEdgeFound = false;
        std::vector<FaceHandle> nextEdgeFaces;

        for (auto e : mesh.edges(nextVertex)) {
            if (ranges::distance(mesh.faces(e)) != 2) {
                // non-manifold edge
                continue;
            }

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
            return {};
        }

        if (nextEdge == edges[0]) {
            // loop found
            return edges;
        }

        if (ranges::find(edges, nextEdge) != edges.end()) {
            // 9 loop
            return {};
        }

        edges.push_back(nextEdge);
        edge = nextEdge;
        vertex = nextVertex;
    }
}

} // namespace Mesh
} // namespace Lattice
