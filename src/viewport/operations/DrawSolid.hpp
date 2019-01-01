#pragma once
#include "../../gl/Shader.hpp"

namespace Lattice {

class VAO;
class Camera;

class DrawSolid {
public:
    DrawSolid();

    void draw(const SP<VAO>& vao, const glm::dmat4 &matrix, const Camera& camera,
              glm::dvec3 diffuse, glm::dvec3 ambient);

private:
    Shader _shader;
};

} // namespace Lattice
