#include "DrawCircle.hpp"
#include "../../gl/PointVAO.hpp"
#include "../../resource/Resource.hpp"
#include "../../support/Location.hpp"
#include "../../support/Camera.hpp"

namespace Lattice::Renderer {

DrawCircle::DrawCircle() :
    _shader(Resource::read("src/renderer/operations/DrawCircle.vert"), Resource::read("src/renderer/operations/DrawCircle.geom"), Resource::read("src/renderer/operations/DrawCircle.frag"))
{
}

void DrawCircle::draw(const SP<GL::PointVAO> &vao, const glm::dmat4 &matrix, const Camera &camera, double width, glm::dvec3 color, double zOffset) {
    _shader.bind();
    _shader.setUniform("MVP", camera.worldToScreenMatrix() * matrix);
    _shader.setUniform("viewportSize", camera.viewSize());
    _shader.setUniform("width", width);
    _shader.setUniform("color", color);
    _shader.setUniform("zOffset", zOffset);
    vao->draw();
}

} // namespace Lattice
