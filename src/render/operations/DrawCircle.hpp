#pragma once
#include "../../gl/Shader.hpp"

namespace Lattice {
class Camera;
}

namespace Lattice::GL {
class VAO;
}

namespace Lattice::Render {

class DrawCircle final {
public:
    DrawCircle();
    void draw(const SP<GL::VAO>& vao, const glm::dmat4 &matrix, const SP<Camera>& camera,
              double width, glm::dvec3 color, bool useVertexColor = false, double zOffset = -0.00002);

private:
    GL::Shader _shader;
};

} // namespace Lattice

