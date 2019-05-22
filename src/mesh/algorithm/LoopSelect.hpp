#pragma once
#include "../Mesh.hpp"

namespace Lattice {
namespace Mesh {

class LoopSelect {
public:
    LoopSelect(EdgeHandle edge) : edge(edge) {}

    void perform(Mesh& mesh) const;

    EdgeHandle edge;
};

} // namespace Mesh
} // namespace Lattice
