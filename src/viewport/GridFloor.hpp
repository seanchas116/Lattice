#pragma once

#include "../support/Pointer.hpp"
#include <glm/glm.hpp>

namespace Lattice {
class Camera;
}

namespace Lattice::GL {
class LineVAO;
}

namespace Lattice::Viewport {

class Operations;

class GridFloor final {
public:
    GridFloor();

    void draw(const SP<Operations>& operations, const Camera& camera);

private:
    SP<GL::LineVAO> _vao;
};

} // namespace Lattice
