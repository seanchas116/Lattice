#pragma once

#include "../support/Pointer.hpp"
#include <glm/glm.hpp>

namespace Lattice {

class LineVAO;
class Shaders;
class Projection;

class GridFloor final {
public:
    GridFloor();

    void draw(const SP<Shaders>& shaders, const glm::mat4& viewMatrix, const Projection& projection);

private:
    SP<LineVAO> _vao;
};

} // namespace Lattice
