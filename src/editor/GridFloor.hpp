#pragma once

#include "../support/Pointer.hpp"
#include "../render/Renderable.hpp"
#include "../gl/VertexBuffer.hpp"
#include <glm/glm.hpp>

namespace Lattice {
class Camera;
}

namespace Lattice::GL {
class LineVAO;
}

namespace Lattice::Editor {

class GridFloor final : public Render::Renderable {
    Q_OBJECT
public:
    GridFloor();

    void draw(const SP<Render::Operations>& operations, const SP<Camera>& camera) override;

private:
    SP<GL::VertexBuffer<GL::Vertex>> _vbo;
    SP<GL::LineVAO> _vao;
    SP<GL::LineVAO> _xAxisVAO;
    SP<GL::LineVAO> _zAxisVAO;
};

} // namespace Lattice
