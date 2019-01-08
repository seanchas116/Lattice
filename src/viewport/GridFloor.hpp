#pragma once

#include "../support/Pointer.hpp"
#include "../renderer/Renderable.hpp"
#include <glm/glm.hpp>

namespace Lattice {
class Camera;
}

namespace Lattice::GL {
class VertexBuffer;
class LineVAO;
}

namespace Lattice::Viewport {

class GridFloor final : public Renderer::Renderable {
public:
    GridFloor();

    void draw(const SP<Renderer::Operations>& operations, const Camera& camera) override;

private:
    SP<GL::VertexBuffer> _vbo;
    SP<GL::LineVAO> _vao;
    SP<GL::LineVAO> _xAxisVAO;
    SP<GL::LineVAO> _zAxisVAO;
};

} // namespace Lattice
