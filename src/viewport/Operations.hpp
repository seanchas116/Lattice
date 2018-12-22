#pragma once

#include "./operations/DrawCircle.hpp"
#include "./operations/DrawLine.hpp"
#include "./operations/DrawSolid.hpp"

namespace Lattice {

class Operations {
public:
    Operations() {}
    DrawCircle drawCircle;
    DrawLine drawLine;
    DrawSolid drawSolid;
};

} // namespace Lattice
