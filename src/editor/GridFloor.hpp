#pragma once

#include "../support/Shorthands.hpp"
#include "../viewport/Renderable.hpp"
#include "../draw/Vertex.hpp"
#include <glm/glm.hpp>

namespace Lattice {
class Camera;

namespace GL {
template <typename T> class VertexBuffer;
class IndexBuffer;
}

namespace Editor {

class GridFloor final : public Viewport::Renderable {
    Q_OBJECT
public:
    GridFloor();

    int normalAxis() const { return _normalAxis; }
    void setNormalAxis(int axis);

    void draw(const SP<Draw::Operations>& operations, const SP<Camera>& camera) override;

private:
    SP<GL::VertexBuffer<Draw::PointLineVertex>> _vbo;
    SP<GL::IndexBuffer> _indexBuffer;
    SP<GL::VAO> _vao;
    SP<GL::IndexBuffer> _yAxisIndexBuffer;
    SP<GL::IndexBuffer> _zAxisIndexBuffer;
    SP<GL::VAO> _yAxisVAO;
    SP<GL::VAO> _zAxisVAO;
    int _normalAxis = 1;
};

}
} // namespace Lattice
