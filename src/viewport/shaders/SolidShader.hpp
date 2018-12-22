#pragma once
#include "../../gl/Shader.hpp"

namespace Lattice {

class SolidShader : public Shader {
public:
    SolidShader();

    void setDiffuse(glm::vec3 diffuse);
    void setAmbient(glm::vec3 ambient);
    void setMVPMatrix(glm::mat4 MV, glm::mat4 MVP);
};

} // namespace Lattice
