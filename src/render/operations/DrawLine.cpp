#include "DrawLine.hpp"
#include "../../support/Camera.hpp"
#include "../../gl/LineVAO.hpp"
#include "../../resource/Resource.hpp"

namespace Lattice::Render {

DrawLine::DrawLine() :
    _shader(Resource::read("src/render/operations/DrawLine.vert"), Resource::read("src/render/operations/DrawLine.geom"), Resource::read("src/render/operations/DrawLine.frag"))
{
}

void DrawLine::draw(const SP<GL::LineVAO> &vao, const glm::dmat4 &matrix, const Camera &camera, double width, glm::dvec3 color, double zOffset) {
    _shader.bind();
    _shader.setUniform("MV", camera.worldToCameraMatrix() * matrix);
    _shader.setUniform("P", camera.cameraToScrenMatrix());
    _shader.setUniform("viewportSize", camera.viewSize());
    _shader.setUniform("zNear", camera.zNear());
    _shader.setUniform("width", width);
    _shader.setUniform("color", color);
    _shader.setUniform("zOffset", zOffset);
    vao->draw();
}

} // namespace Lattice
