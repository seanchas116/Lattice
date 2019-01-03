#pragma once

#include "../support/Pointer.hpp"
#include <glm/glm.hpp>

namespace Lattice {

class LineVAO;
class Camera;

}

namespace Lattice::Viewport {

class Operations;

class GridFloor final {
public:
    GridFloor();

    void draw(const SP<Operations>& operations, const Camera& camera);

private:
    SP<LineVAO> _vao;
};

} // namespace Lattice
