#include "DrawCircle.hpp"
#include "../../oldgl/PointVAO.hpp"
#include "../../resource/Resource.hpp"
#include "../../support/Location.hpp"
#include "../../support/Camera.hpp"

namespace Lattice::Render {

DrawCircle::DrawCircle() :
    _shader(Resource::read("src/render/operations/DrawCircle.vert"), Resource::read("src/render/operations/DrawCircle.geom"), Resource::read("src/render/operations/DrawCircle.frag"))
{
}

void DrawCircle::draw(const SP<OldGL::PointVAO> &vao, const glm::dmat4 &matrix, const SP<Camera> &camera, double width, glm::dvec3 color, bool useVertexColor, double zOffset) {
    _shader.bind();
    _shader.setUniform("MVP", camera->worldToScreenMatrix() * matrix);
    _shader.setUniform("viewportSize", camera->viewSize());
    _shader.setUniform("width", width);
    _shader.setUniform("color", color);
    _shader.setUniform("useVertexColor", useVertexColor);
    _shader.setUniform("zOffset", zOffset);
    vao->draw();
}

} // namespace Lattice
