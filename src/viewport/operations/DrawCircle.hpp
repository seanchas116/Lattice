#pragma once
#include "../../gl/Shader.hpp"

namespace Lattice {

class PointVAO;
class Camera;

class DrawCircle {
public:
    DrawCircle();
    void draw(const SP<PointVAO>& vao, const glm::dmat4 &matrix, const Camera& camera,
              double width, glm::dvec3 color, double zOffset = -0.00002);

private:
    Shader _shader;
};

} // namespace Lattice

