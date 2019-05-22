#include "SplitSharpEdges.hpp"

namespace Lattice {
namespace Mesh {

void SplitSharpEdges::perform(Mesh &mesh) const {
    for (auto v : mesh.vertices()) {
        int nSharpEdges = 0;
        for (auto e : mesh.edges(v)) {
            if (!mesh.isSmooth(e) || ranges::distance(mesh.faces(e)) >= 3) {
                ++nSharpEdges;
            }
        }
        if (nSharpEdges <= 1) {
            continue;
        }
    }
}

} // namespace Mesh
} // namespace Lattice
