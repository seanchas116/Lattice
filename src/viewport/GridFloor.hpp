#pragma once

#include "../support/Pointer.hpp"
#include <glm/glm.hpp>

namespace Lattice {
class Camera;
}

namespace Lattice::GL {
class VertexBuffer;
class LineVAO;
}

namespace Lattice::Viewport {

class Operations;

class GridFloor final {
public:
    GridFloor();

    void draw(const SP<Operations>& operations, const Camera& camera);

private:
    SP<GL::VertexBuffer> _vbo;
    SP<GL::LineVAO> _vao;
    SP<GL::LineVAO> _xAxisVAO;
    SP<GL::LineVAO> _zAxisVAO;
};

} // namespace Lattice
