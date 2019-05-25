#pragma once
#include "../Mesh.hpp"

namespace Lattice {
namespace Mesh {

class FindLoop {
public:
    FindLoop(EdgeHandle edge) : edge(edge) {}

    std::vector<EdgeHandle> perform(const Mesh& mesh) const;

    EdgeHandle edge;
};

} // namespace Mesh
} // namespace Lattice
