#include "LoopCut.hpp"
#include "CutEdge.hpp"
#include "../../support/Shorthands.hpp"
#include <range/v3/algorithm/find.hpp>
#include <range/v3/algorithm/find_if.hpp>

namespace Lattice {
namespace Mesh {

std::vector<VertexHandle> LoopCut::perform(Mesh &mesh) {
    bool isEdgeReverse = false;
    std::vector<std::pair<EdgeHandle, bool>> edges;
    Opt<FaceHandle> lastFace;

    while (true) {
        edges.push_back({edge, isEdgeReverse});

        auto edgeFaces = mesh.faces(edge) | ranges::to_vector;

        if (edgeFaces.size() != 2) {
            return {};
        }
        auto nextFace = (lastFace && *lastFace == edgeFaces[0]) ? edgeFaces[1] : edgeFaces[0];
        lastFace = nextFace;

        auto& nextFaceEdges = mesh.edges(nextFace);

        if (nextFaceEdges.size() != 4) {
            return {};
        }
        size_t edgeIndex = ranges::find(nextFaceEdges, edge) - nextFaceEdges.begin();
        size_t nextEdgeIndex = (edgeIndex + 2) % nextFaceEdges.size();
        auto nextEdge = nextFaceEdges[nextEdgeIndex];

        bool edgeDirection = mesh.vertices(nextFace)[edgeIndex] == mesh.vertices(edge)[0];
        bool nextEdgeDirection = mesh.vertices(nextFace)[nextEdgeIndex] == mesh.vertices(nextEdge)[0];
        if (edgeDirection == nextEdgeDirection) {
            isEdgeReverse = !isEdgeReverse;
        }

        if (nextEdge == edges[0].first) {
            // loop found
            break;
        }
        if (ranges::find_if(edges, [&](auto& pair) { return pair.first == nextEdge; }) != edges.end()) {
            // 9-like loop
            return {};
        }
        edge = nextEdge;
    }

    std::vector<VertexHandle> vertices;
    vertices.reserve(edges.size());
    for (auto& [edge, isReverse] : edges) {
        auto v = CutEdge(edge, isReverse ? (1.0 - cutPosition) : cutPosition).perform(mesh);
        vertices.push_back(v);
    }
    for (size_t i = 0; i < vertices.size(); ++i) {
        auto v0 = vertices[i];
        auto v1 = vertices[(i + 1) % vertices.size()];
        mesh.addEdge({v0, v1});
    }

    return vertices;
}

} // namespace Mesh
} // namespace Lattice
