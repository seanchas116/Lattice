#pragma once
#include "../gl/Shader.hpp"

namespace Lattice {
class OldCamera;

namespace GL {
class VAO;
}

namespace Draw {

class DrawCircle final {
public:
    inline static constexpr double defaultZOffset = -0.00002;

    DrawCircle();

    void draw(const SP<GL::VAO>& vao, const glm::dmat4 &matrix, const SP<OldCamera>& camera,
              double width, glm::vec4 color, bool useVertexColor = false, double zOffset = defaultZOffset);
    void draw2D(const SP<GL::VAO>& vao, const glm::dmat4 &matrix, glm::ivec2 viewportSize,
                double width, glm::vec4 color, bool useVertexColor = false);

private:
    GL::Shader _shader;
};

}
} // namespace Lattice

