#pragma once
#include "Point.hpp"
#include "../support/Shorthands.hpp"
#include <glm/glm.hpp>

namespace Lattice {

namespace GL {
class Shader;
class VAO;
template <typename T> class VertexBuffer;
class IndexBuffer;
}

namespace Drawable {

class LinesGeometry {
public:
    LinesGeometry();

    void setPoints(const std::vector<Point>& points);
    void setLines(const std::vector<std::array<uint32_t, 2>>& lines);
    void setLineStrips(const std::vector<std::vector<uint32_t>>& lineStrips);

private:
    friend class LinesDrawable;

    SP<GL::VertexBuffer<Point>> _vbo;
    SP<GL::IndexBuffer> _ibo;
    SP<GL::VAO> _vao;
};

} // namespace Drawable
} // namespace Lattice
