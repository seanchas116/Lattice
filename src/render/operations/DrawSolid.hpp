#pragma once
#include "../../gl/Shader.hpp"

namespace Lattice {
class Camera;
}

namespace Lattice::OldGL {
class VAO;
}

namespace Lattice::Render {

class DrawSolid final {
public:
    DrawSolid();

    void draw(const SP<OldGL::VAO>& vao, const glm::dmat4 &matrix, const SP<Camera>& camera,
              glm::dvec3 diffuse, glm::dvec3 ambient);

private:
    OldGL::Shader _shader;
};

} // namespace Lattice
