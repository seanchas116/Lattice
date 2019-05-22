#pragma once
#include "../Mesh.hpp"

namespace Lattice {
namespace Mesh {

class SplitSharpEdges {
public:
    void perform(Mesh& mesh) const;
};

} // namespace Mesh
} // namespace Lattice
