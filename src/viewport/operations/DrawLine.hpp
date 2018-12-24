#pragma once

#include "../../gl/Shader.hpp"

namespace Lattice {

class LineVAO;
class Projection;

class DrawLine {
public:
    DrawLine();

    void draw(const SP<LineVAO>& vao, const glm::mat4& matrix, const Projection& projection,
              float width, glm::vec3 color, float zOffset = -0.00001f);

private:
    Shader _shader;
};

} // namespace Lattice
