#pragma once

#include "../gl/Shader.hpp"

namespace Lattice {
class Camera;

namespace GL {
class VAO;
}

namespace Draw {

class DrawLine final {
  public:
    inline static constexpr double defaultZOffset = -0.00001;

    DrawLine();

    void draw(const SP<GL::VAO> &vao, const glm::dmat4 &matrix, const Camera &camera,
              double width, glm::vec4 color, bool useVertexColor = false, double zOffset = defaultZOffset);

  private:
    GL::Shader _shader;
};

} // namespace Draw
} // namespace Lattice
