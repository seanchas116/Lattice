#pragma once

#include "../../gl/Shader.hpp"

namespace Lattice {
class Camera;
}

namespace Lattice::GL {
class VAO;
}

namespace Lattice::Render {

class DrawLine final {
public:
    inline static constexpr double defaultZOffset = -0.00001;

    DrawLine();

    void draw(const SP<GL::VAO>& vao, const glm::dmat4 &matrix, const SP<Camera>& camera,
              double width, glm::dvec3 color, bool useVertexColor = false, double zOffset = defaultZOffset);

private:
    GL::Shader _shader;
};

} // namespace Lattice
