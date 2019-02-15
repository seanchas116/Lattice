#pragma once

#include "../support/Shorthands.hpp"
#include "../render/RenderableObject.hpp"
#include "../gl/VertexBuffer.hpp"
#include <glm/glm.hpp>

namespace Lattice {
class Camera;
}

namespace Lattice::GL {
class VAO;
class IndexBuffer;
}

namespace Lattice::Editor {

class GridFloor final : public Render::RenderableObject {
    Q_OBJECT
public:
    GridFloor();

    void draw(const SP<Render::Operations>& operations, const SP<Camera>& camera) override;

private:
    SP<GL::VertexBuffer<GL::Vertex>> _vbo;
    SP<GL::IndexBuffer> _indexBuffer;
    SP<GL::VAO> _vao;
    SP<GL::IndexBuffer> _xAxisIndexBuffer;
    SP<GL::IndexBuffer> _zAxisIndexBuffer;
    SP<GL::VAO> _xAxisVAO;
    SP<GL::VAO> _zAxisVAO;
};

} // namespace Lattice
