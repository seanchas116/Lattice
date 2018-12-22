#pragma once

#include "./shaders/CircleShader.hpp"
#include "./shaders/ThickLineShader.hpp"
#include "./shaders/SolidShader.hpp"

namespace Lattice {

class Shaders {
public:
    Shaders() {}
    CircleShader circleShader;
    ThickLineShader thickLineShader;
    SolidShader solidShader;
};

} // namespace Lattice
