#pragma once

#include "./Clear.hpp"
#include "./Copy.hpp"
#include "./DrawCircle.hpp"
#include "./DrawLine.hpp"
#include "./DrawMaterial.hpp"
#include "./DrawUnicolor.hpp"

namespace Lattice {
namespace Draw {

class Operations final {
  public:
    Operations() = default;

    Clear clear;
    Copy copy;
    DrawCircle drawCircle;
    DrawLine drawLine;
    DrawMaterial drawMaterial;
    DrawUnicolor drawUnicolor;
};

} // namespace Draw
} // namespace Lattice
