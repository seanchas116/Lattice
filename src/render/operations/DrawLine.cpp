#include "DrawLine.hpp"
#include "../../support/Camera.hpp"
#include "../../oldgl/LineVAO.hpp"
#include "../../resource/Resource.hpp"

namespace Lattice::Render {

DrawLine::DrawLine() :
    _shader(Resource::read("src/render/operations/DrawLine.vert"), Resource::read("src/render/operations/DrawLine.geom"), Resource::read("src/render/operations/DrawLine.frag"))
{
}

void DrawLine::draw(const SP<OldGL::LineVAO> &vao, const glm::dmat4 &matrix, const SP<Camera> &camera, double width, glm::dvec3 color, bool useVertexColor, double zOffset) {
    _shader.bind();
    _shader.setUniform("MV", camera->worldToCameraMatrix() * matrix);
    _shader.setUniform("P", camera->cameraToScreenMatrix());
    _shader.setUniform("viewportSize", camera->viewSize());
    _shader.setUniform("zNear", camera->projection() == Camera::Projection::Perspective ? camera->zNear() : -10000.0); // TODO: specify depth in better way
    _shader.setUniform("width", width);
    _shader.setUniform("color", color);
    _shader.setUniform("useVertexColor", useVertexColor);
    _shader.setUniform("zOffset", zOffset);
    vao->draw();
}

} // namespace Lattice
