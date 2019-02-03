#pragma once

#include "../../oldgl/Shader.hpp"

namespace Lattice {
class Camera;
}

namespace Lattice::OldGL {
class LineVAO;
}

namespace Lattice::Render {

class DrawLine final {
public:
    DrawLine();

    void draw(const SP<OldGL::LineVAO>& vao, const glm::dmat4 &matrix, const SP<Camera>& camera,
              double width, glm::dvec3 color, bool useVertexColor = false, double zOffset = -0.00001);

private:
    OldGL::Shader _shader;
};

} // namespace Lattice
