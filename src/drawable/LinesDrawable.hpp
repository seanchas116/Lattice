#pragma once
#include "Point.hpp"
#include "../support/Shorthands.hpp"
#include "../support/SharedResourceBag.hpp"
#include <glm/glm.hpp>
#include <vector>

namespace Lattice {

class Camera;

namespace GL {
class Shader;
class VAO;
template <typename T> class VertexBuffer;
class IndexBuffer;
}

namespace Drawable {

class LinesDrawable {
public:
    inline static constexpr double defaultZOffset = -0.00002;

    LinesDrawable();

    void setWidth(double width) { _width = width; }
    void setZOffset(double zOffset) { _zOffset = zOffset; }
    void setColor(glm::vec4 color) { _color = color ; }
    void setUseVertexColor(bool use) { _useVertexColor = use; }
    void setPoints(const std::vector<Point>& points);
    void setLines(const std::vector<std::array<uint32_t, 2>>& lines);
    void setLineStrips(const std::vector<std::vector<uint32_t>>& lineStrips);

    void draw(SharedResourceBag& resourceBag, const glm::dmat4 &matrix, const SP<Camera>& camera);

private:
    static const SP<GL::Shader>& shader();
    SP<GL::VertexBuffer<Point>> _vbo;
    SP<GL::IndexBuffer> _ibo;
    SP<GL::VAO> _vao;
    double _width = 1;
    double _zOffset = defaultZOffset;
    glm::vec4 _color {0};
    bool _useVertexColor = true;
};

} // namespace Drawable
} // namespace Lattice
