#pragma once

#include "../support/Shorthands.hpp"
#include "../viewport/RenderableObject.hpp"
#include "../draw/Vertex.hpp"
#include <glm/glm.hpp>

namespace Lattice {
class Camera;

namespace GL {
template <typename T> class VertexBuffer;
class IndexBuffer;
}

namespace Editor {

class GridFloor final : public Viewport::RenderableObject {
    Q_OBJECT
public:
    GridFloor();

    void draw(const SP<Draw::Operations>& operations, const SP<Camera>& camera) override;

private:
    SP<GL::VertexBuffer<Draw::Vertex>> _vbo;
    SP<GL::IndexBuffer> _indexBuffer;
    SP<GL::VAO> _vao;
    SP<GL::IndexBuffer> _xAxisIndexBuffer;
    SP<GL::IndexBuffer> _zAxisIndexBuffer;
    SP<GL::VAO> _xAxisVAO;
    SP<GL::VAO> _zAxisVAO;
};

}
} // namespace Lattice
