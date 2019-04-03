#pragma once
#include "../support/Shorthands.hpp"
#include <glm/glm.hpp>
#include <vector>

namespace Lattice {
namespace GL {
class Shader;
class VAO;
}

namespace Drawable {

struct Point {
    glm::vec3 position;
    glm::vec3 color;
};

class PointsDrawable {
public:
    PointsDrawable();

    void setWidth(double width);
    void setZOffset(double zOffset);
    void setPoints(const std::vector<Point>& points);

private:
    SP<GL::Shader> _shader;
};

} // namespace Drawable
} // namespace Lattice
