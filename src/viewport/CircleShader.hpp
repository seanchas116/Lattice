#pragma once
#include "../gl/Shader.hpp"

namespace Lattice {

class CircleShader : public Shader {
public:
    CircleShader();

    void setWidth(float width);
    void setColor(glm::vec3 color);
    void setMVPMatrix(glm::mat4 MVP);
    void setViewportSize(glm::vec2 size);
};

} // namespace Lattice

