#pragma once
#include "../../gl/Shader.hpp"

namespace Lattice {

class VAO;
class Camera;

}

namespace Lattice::Viewport {

class DrawSolid final {
public:
    DrawSolid();

    void draw(const SP<VAO>& vao, const glm::dmat4 &matrix, const Camera& camera,
              glm::dvec3 diffuse, glm::dvec3 ambient);

private:
    Shader _shader;
};

} // namespace Lattice
