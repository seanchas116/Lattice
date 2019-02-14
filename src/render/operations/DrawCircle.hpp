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
    inline static constexpr double defaultZOffset = -0.00002;

    DrawCircle();

    void draw(const SP<GL::VAO>& vao, const glm::dmat4 &matrix, const SP<Camera>& camera,
              double width, glm::vec4 color, bool useVertexColor = false, double zOffset = defaultZOffset);

private:
    GL::Shader _shader;
};

} // namespace Lattice

