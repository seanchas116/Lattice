#pragma once

#include "../../gl/Shader.hpp"

namespace Lattice {
class Camera;
}

namespace Lattice::GL {
class LineVAO;
}

namespace Lattice::Render {

class DrawLine final {
public:
    DrawLine();

    void draw(const SP<GL::LineVAO>& vao, const glm::dmat4 &matrix, const Camera& camera,
              double width, glm::dvec3 color, double zOffset = -0.00001);

private:
    GL::Shader _shader;
};

} // namespace Lattice
