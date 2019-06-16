#include "FindBelt.hpp"
#include "../../support/Shorthands.hpp"
#include <range/v3/algorithm/find.hpp>
#include <range/v3/algorithm/find_if.hpp>

namespace Lattice {
namespace Mesh {

std::vector<BeltElement> findBelt(const Mesh &mesh, EdgeHandle edge) {
    bool isEdgeReverse = false;
    std::vector<BeltElement> belt;
    Opt<FaceHandle> lastFace;

    while (true) {
        auto edgeFaces = mesh.faces(edge) | ranges::to_vector;

        if (edgeFaces.size() != 2) {
            return {};
        }
        auto nextFace = (lastFace && *lastFace == edgeFaces[0]) ? edgeFaces[1] : edgeFaces[0];
        lastFace = nextFace;

        belt.push_back({edge, nextFace, isEdgeReverse});

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

        if (nextEdge == belt[0].edge) {
            // loop found
            return belt;
        }
        if (ranges::find_if(belt, [&](auto& elem) { return elem.edge == nextEdge; }) != belt.end()) {
            // 9-like loop
            return {};
        }
        edge = nextEdge;
    }
}

} // namespace Mesh
} // namespace Lattice
