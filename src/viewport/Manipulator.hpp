#pragma once

#include <glm/glm.hpp>
#include "Shaders.hpp"
#include "../support/Projection.hpp"

namespace Lattice {

class Manipulator {
public:
    Manipulator();

    void drawVertices(const SP<Shaders>& shaders, const glm::mat4& viewMatrix, const Projection& projection);
};

} // namespace Lattice
