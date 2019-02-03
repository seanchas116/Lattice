#pragma once
#include "../../oldgl/Shader.hpp"

namespace Lattice {
class Camera;
}

namespace Lattice::OldGL {
class PointVAO;
}

namespace Lattice::Render {

class DrawCircle final {
public:
    DrawCircle();
    void draw(const SP<OldGL::PointVAO>& vao, const glm::dmat4 &matrix, const SP<Camera>& camera,
              double width, glm::dvec3 color, bool useVertexColor = false, double zOffset = -0.00002);

private:
    OldGL::Shader _shader;
};

} // namespace Lattice

