#pragma once
#include "Point.hpp"
#include "../support/Shorthands.hpp"
#include "../support/SingletonBag.hpp"
#include <glm/glm.hpp>
#include <vector>

namespace Lattice {

class Camera;

namespace GL {
class Shader;
class VAO;
template <typename T> class VertexBuffer;
}

namespace Drawable {

class PointsDrawable {
public:
    inline static constexpr double defaultZOffset = -0.00002;

    PointsDrawable();

    void setWidth(double width) { _width = width; }
    void setZOffset(double zOffset) { _zOffset = zOffset; }
    void setPoints(const std::vector<Point>& points);

    void draw(SingletonBag& singletonBag, const glm::dmat4 &matrix, const SP<Camera>& camera);
    void draw2D(SingletonBag& singletonBag, const glm::dmat4 &matrix, glm::ivec2 viewportSize);

private:
    SP<GL::VertexBuffer<Point>> _vbo;
    SP<GL::VAO> _vao;
    double _width = 1;
    double _zOffset = defaultZOffset;
};

} // namespace Drawable
} // namespace Lattice
