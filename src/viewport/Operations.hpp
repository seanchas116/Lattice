#pragma once

#include "./operations/DrawCircle.hpp"
#include "./operations/DrawLine.hpp"
#include "./operations/DrawSolid.hpp"
#include "./operations/DrawMaterial.hpp"

namespace Lattice {

class Operations {
public:
    Operations() = default;

    DrawCircle drawCircle;
    DrawLine drawLine;
    DrawSolid drawSolid;
    DrawMaterial drawMaterial;
};

} // namespace Lattice
