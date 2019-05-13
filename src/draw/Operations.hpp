#pragma once

#include "./Clear.hpp"
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
    DrawCircle drawCircle;
    DrawLine drawLine;
    DrawMaterial drawMaterial;
    DrawUnicolor drawUnicolor;
};

}
}
