#pragma once
#include "../../gl/Shader.hpp"

namespace Lattice {
class Camera;
}

namespace Lattice::GL {
class VAO;
}

namespace Lattice::Viewport {

class DrawSolid final {
public:
    DrawSolid();

    void draw(const SP<GL::VAO>& vao, const glm::dmat4 &matrix, const Camera& camera,
              glm::dvec3 diffuse, glm::dvec3 ambient);

private:
    GL::Shader _shader;
};

} // namespace Lattice
