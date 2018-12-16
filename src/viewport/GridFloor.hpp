#pragma once

#include "../support/Pointer.hpp"

namespace Lattice {

class LineMesh;

class GridFloor final {
public:
    GridFloor();

    void draw();

private:
    SP<LineMesh> _mesh;
};

} // namespace Lattice
