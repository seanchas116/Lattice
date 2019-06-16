#include "LoopCut.hpp"
#include "CutEdge.hpp"
#include "FindBelt.hpp"
#include <range/v3/algorithm/find.hpp>
#include <range/v3/algorithm/find_if.hpp>

namespace Lattice {
namespace Mesh {

std::vector<VertexHandle> loopCut(Mesh &mesh, EdgeHandle edge, float cutPosition) {
    auto belt = findBelt(mesh, edge);

    std::vector<VertexHandle> vertices;
    vertices.reserve(belt.size());
    for (auto& [edge, face, isReverse] : belt) {
        auto v = cutEdge(mesh, edge, isReverse ? (1.f - cutPosition) : cutPosition);
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
