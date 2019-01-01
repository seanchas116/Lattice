#pragma once

#include "../../gl/Shader.hpp"

namespace Lattice {

class LineVAO;
class Camera;

class DrawLine {
public:
    DrawLine();

    void draw(const SP<LineVAO>& vao, const glm::dmat4 &matrix, const Camera& camera,
              double width, glm::dvec3 color, double zOffset = -0.00001);

private:
    Shader _shader;
};

} // namespace Lattice
