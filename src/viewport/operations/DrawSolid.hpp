#pragma once
#include "../../gl/Shader.hpp"

namespace Lattice {

class VAO;
class Projection;

class DrawSolid {
public:
    DrawSolid();

    void draw(const SP<VAO>& vao, const glm::mat4& matrix, const Projection& projection,
              glm::vec3 diffuse, glm::vec3 ambient);

private:
    Shader _shader;
};

} // namespace Lattice
