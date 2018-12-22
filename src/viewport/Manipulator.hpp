#pragma once

#include <glm/glm.hpp>
#include "Operations.hpp"
#include "../support/Projection.hpp"

namespace Lattice {

class Manipulator {
public:
    Manipulator();

    void drawVertices(const SP<Operations>& operations, const glm::mat4& viewMatrix, const Projection& projection);
};

} // namespace Lattice
