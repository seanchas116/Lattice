#pragma once

#include "../support/Pointer.hpp"
#include "../render/Renderable.hpp"
#include <glm/glm.hpp>

namespace Lattice {
class Camera;
}

namespace Lattice::OldGL {
class VertexBuffer;
class LineVAO;
}

namespace Lattice::Editor {

class GridFloor final : public Render::Renderable {
    Q_OBJECT
public:
    GridFloor();

    void draw(const SP<Render::Operations>& operations, const SP<Camera>& camera) override;

private:
    SP<OldGL::VertexBuffer> _vbo;
    SP<OldGL::LineVAO> _vao;
    SP<OldGL::LineVAO> _xAxisVAO;
    SP<OldGL::LineVAO> _zAxisVAO;
};

} // namespace Lattice
