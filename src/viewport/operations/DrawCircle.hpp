#pragma once
#include "../../gl/Shader.hpp"

namespace Lattice {

class PointVAO;
class Projection;

class DrawCircle {
public:
    DrawCircle();
    void draw(const SP<PointVAO>& vao, const glm::mat4& matrix, const Projection& projection,
              float width, glm::vec3 color, float zOffset = -0.00002f);

private:
    Shader _shader;
};

} // namespace Lattice

