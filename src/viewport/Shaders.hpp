#pragma once

#include "./shaders/DrawCircle.hpp"
#include "./shaders/DrawLine.hpp"
#include "./shaders/DrawSolid.hpp"

namespace Lattice {

class Shaders {
public:
    Shaders() {}
    DrawCircle drawCircle;
    DrawLine drawLine;
    DrawSolid drawSolid;
};

} // namespace Lattice
