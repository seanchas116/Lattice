#include "DrawCircle.hpp"
#include "../../gl/PointVAO.hpp"
#include "../../resource/Resource.hpp"
#include "../../support/Location.hpp"
#include "../../support/OldCamera.hpp"
#include "../../support/Projection.hpp"

namespace Lattice {

DrawCircle::DrawCircle() :
    _shader(readResource("src/viewport/operations/DrawCircle.vert"), readResource("src/viewport/operations/DrawCircle.geom"), readResource("src/viewport/operations/DrawCircle.frag"))
{
}

void DrawCircle::draw(const SP<PointVAO> &vao, const glm::mat4 &matrix, const Projection &projection, float width, glm::vec3 color, float zOffset) {
    _shader.bind();
    _shader.setUniform("MVP", projection.projectionMatrix() * matrix);
    _shader.setUniform("viewportSize", projection.viewSize());
    _shader.setUniform("width", width);
    _shader.setUniform("color", color);
    _shader.setUniform("zOffset", zOffset);
    vao->draw();
}

} // namespace Lattice
