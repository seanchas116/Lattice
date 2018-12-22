#pragma once

#include "CircleShader.hpp"
#include "ThickLineShader.hpp"
#include "SolidShader.hpp"

namespace Lattice {

class Shaders {
public:
    Shaders() {}
    CircleShader circleShader;
    ThickLineShader thickLineShader;
    SolidShader solidShader;
};

} // namespace Lattice
