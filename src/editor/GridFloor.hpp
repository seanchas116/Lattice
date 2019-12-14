#pragma once

#include "../draw/Vertex.hpp"
#include "../support/Shorthands.hpp"
#include "../viewport/Renderable.hpp"
#include <glm/glm.hpp>

namespace Lattice {

namespace GL {
template <typename T>
class VertexBuffer;
class IndexBuffer;
} // namespace GL

namespace Editor {

class GridFloor final : public Viewport::Renderable {
    Q_OBJECT
  public:
    GridFloor();

    void draw(const Viewport::DrawEvent &event) override;

  private:
    SP<GL::VertexBuffer<Draw::PointLineVertex>> _vbo;
    SP<GL::IndexBuffer> _indexBuffer;
    SP<GL::VAO> _vao;
    SP<GL::IndexBuffer> _yAxisIndexBuffer;
    SP<GL::IndexBuffer> _zAxisIndexBuffer;
    SP<GL::VAO> _yAxisVAO;
    SP<GL::VAO> _zAxisVAO;
};

} // namespace Editor
} // namespace Lattice
