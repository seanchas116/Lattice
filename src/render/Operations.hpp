#pragma once

#include "./operations/DrawCircle.hpp"
#include "./operations/DrawLine.hpp"
#include "./operations/DrawSolid.hpp"
#include "./operations/DrawMaterial.hpp"
#include "./operations/DrawUnicolor.hpp"
#include "../support/SharedResourceBag.hpp"

namespace Lattice {
namespace Render {

class Operations final {
public:
    Operations() = default;

    DrawCircle drawCircle;
    DrawLine drawLine;
    DrawSolid drawSolid;
    DrawMaterial drawMaterial;

    DrawUnicolor drawUnicolor;

    SharedResourceBag resourceBag;
};

}
}
