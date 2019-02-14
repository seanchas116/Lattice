#pragma once
#include "../../gl/Shader.hpp"

namespace Lattice {
class Camera;
}

namespace Lattice::GL {
class VAO;
}

namespace Lattice::Render {

class Pickable;

class DrawUnicolor final {
public:
    DrawUnicolor();

    void draw(const SP<GL::VAO>& vao, const glm::dmat4 &matrix, const SP<Camera>& camera, glm::vec4 color);

private:
    GL::Shader _shader;
};

} // namespace Lattice
