#pragma once

#include "../../gl/Shader.hpp"

namespace Lattice {

class ThickLineShader : public Shader {
public:
    ThickLineShader();

    void setWidth(float width);
    void setColor(glm::vec3 color);
    void setMVMatrix(glm::mat4 MV);
    void setPMatrix(glm::mat4 P);
    void setViewportSize(glm::vec2 size);
};

} // namespace Lattice
