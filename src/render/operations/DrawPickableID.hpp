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

class DrawPickableID final {
public:
    DrawPickableID();

    void draw(const SP<GL::VAO>& vao, const glm::dmat4 &matrix, const SP<Camera>& camera, const SP<Pickable>& pickable);

private:
    GL::Shader _shader;
};

} // namespace Lattice
