#pragma once

#include "../support/Pointer.hpp"

namespace Lattice {

class LineVAO;

class GridFloor final {
public:
    GridFloor();

    void draw();

private:
    SP<LineVAO> _vao;
};

} // namespace Lattice
