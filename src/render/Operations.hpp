#pragma once

#include "./operations/DrawCircle.hpp"
#include "./operations/DrawLine.hpp"
#include "./operations/DrawSolid.hpp"
#include "./operations/DrawMaterial.hpp"
#include "./operations/DrawPickableID.hpp"

namespace Lattice {
namespace Render {

class Operations final {
public:
    Operations() = default;

    DrawCircle drawCircle;
    DrawLine drawLine;
    DrawSolid drawSolid;
    DrawMaterial drawMaterial;

    DrawPickableID drawPickableID;
};

}
}
