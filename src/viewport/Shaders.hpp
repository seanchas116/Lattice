#pragma once

#include "./shaders/DrawCircle.hpp"
#include "./shaders/ThickLineShader.hpp"
#include "./shaders/SolidShader.hpp"

namespace Lattice {

class Shaders {
public:
    Shaders() {}
    DrawCircle drawCircle;
    ThickLineShader thickLineShader;
    SolidShader solidShader;
};

} // namespace Lattice
