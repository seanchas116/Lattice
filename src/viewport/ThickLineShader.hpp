#pragma once

#include "../gl/Shader.hpp"

namespace Lattice {

class ThickLineShader : public Shader {
public:
    ThickLineShader();

    void setWidth(float width);
    void setColor(glm::vec3 color);
    void setMVPMatrix(glm::mat4 MVP);
    void setViewportSize(glm::vec2 size);
};

} // namespace Lattice
