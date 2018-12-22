#pragma once

#include "./shaders/DrawCircle.hpp"
#include "./shaders/DrawLine.hpp"
#include "./shaders/SolidShader.hpp"

namespace Lattice {

class Shaders {
public:
    Shaders() {}
    DrawCircle drawCircle;
    DrawLine drawLine;
    SolidShader solidShader;
};

} // namespace Lattice
