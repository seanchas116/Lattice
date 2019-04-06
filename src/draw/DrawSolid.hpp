#pragma once
#include "../gl/Shader.hpp"

namespace Lattice {
class Camera;

namespace GL {
class VAO;
}

namespace Render {

class DrawSolid final {
public:
    DrawSolid();

    void draw(const SP<GL::VAO>& vao, const glm::dmat4 &matrix, const SP<Camera>& camera,
              glm::dvec3 diffuse, glm::dvec3 ambient);

private:
    GL::Shader _shader;
};

}
} // namespace Lattice
