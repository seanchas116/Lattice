#pragma once
#include "../gl/Shader.hpp"

namespace Lattice {
class OldCamera;

namespace GL {
class VAO;
}

namespace Draw {

class DrawUnicolor final {
public:
    DrawUnicolor();

    void draw(const SP<GL::VAO>& vao, const glm::dmat4 &matrix, const SP<OldCamera>& camera, glm::vec4 color, bool useVertexColor = false);

private:
    GL::Shader _shader;
};

}
} // namespace Lattice
