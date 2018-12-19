#pragma once
#include "../gl/Shader.hpp"

namespace Lattice {

class SolidShader : public Shader {
public:
    SolidShader();

    void setDiffuse(glm::vec3 diffuse);
    void setMVPMatrix(glm::mat4 MVP);
};

} // namespace Lattice
