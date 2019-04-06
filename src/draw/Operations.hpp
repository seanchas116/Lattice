#pragma once

#include "./DrawCircle.hpp"
#include "./DrawLine.hpp"
#include "./DrawSolid.hpp"
#include "./DrawMaterial.hpp"
#include "./DrawUnicolor.hpp"

namespace Lattice {
namespace Draw {

class Operations final {
public:
    Operations() = default;

    DrawCircle drawCircle;
    DrawLine drawLine;
    DrawSolid drawSolid;
    DrawMaterial drawMaterial;

    DrawUnicolor drawUnicolor;
};

}
}
