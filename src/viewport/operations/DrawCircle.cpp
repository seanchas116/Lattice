#include "DrawCircle.hpp"
#include "../../gl/PointVAO.hpp"
#include "../../resource/Resource.hpp"
#include "../../support/Location.hpp"
#include "../../support/Camera.hpp"

namespace Lattice::Viewport {

DrawCircle::DrawCircle() :
    _shader(readResource("src/viewport/operations/DrawCircle.vert"), readResource("src/viewport/operations/DrawCircle.geom"), readResource("src/viewport/operations/DrawCircle.frag"))
{
}

void DrawCircle::draw(const SP<PointVAO> &vao, const glm::dmat4 &matrix, const Camera &camera, double width, glm::dvec3 color, double zOffset) {
    _shader.bind();
    _shader.setUniform("MVP", camera.worldToScreenMatrix() * matrix);
    _shader.setUniform("viewportSize", camera.viewSize());
    _shader.setUniform("width", width);
    _shader.setUniform("color", color);
    _shader.setUniform("zOffset", zOffset);
    vao->draw();
}

} // namespace Lattice
