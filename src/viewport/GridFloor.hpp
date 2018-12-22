#pragma once

#include "../support/Pointer.hpp"
#include <glm/glm.hpp>

namespace Lattice {

class LineVAO;
class Operations;
class Projection;

class GridFloor final {
public:
    GridFloor();

    void draw(const SP<Operations>& operations, const glm::mat4& viewMatrix, const Projection& projection);

private:
    SP<LineVAO> _vao;
};

} // namespace Lattice
