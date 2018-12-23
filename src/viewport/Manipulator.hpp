#pragma once

#include <glm/glm.hpp>
#include "Operations.hpp"
#include "../support/Projection.hpp"

namespace Lattice {

class Manipulator {
public:
    Manipulator();

    void draw(const SP<Operations>& operations, const glm::mat4& viewMatrix, const Projection& projection);

private:
    SP<VAO> _headVAO;
    SP<LineVAO> _bodyVAO;
};

} // namespace Lattice
