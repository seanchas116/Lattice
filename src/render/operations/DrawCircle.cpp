#include "DrawCircle.hpp"
#include "../../gl/VAO.hpp"
#include "../../resource/Resource.hpp"
#include "../../support/Location.hpp"
#include "../../support/Camera.hpp"

namespace Lattice {
namespace Render {

DrawCircle::DrawCircle() :
    _shader(Resource::read("src/render/operations/DrawCircle.vert"), Resource::read("src/render/operations/DrawCircle.geom"), Resource::read("src/render/operations/DrawCircle.frag"))
{
}

void DrawCircle::draw(const SP<GL::VAO> &vao, const glm::dmat4 &matrix, const SP<Camera> &camera, double width, glm::vec4 color, bool useVertexColor, double zOffset) {
    _shader.bind();
    _shader.setUniform("MVP", camera->worldToViewportMatrix() * matrix);
    _shader.setUniform("viewportSize", camera->viewSize());
    _shader.setUniform("width", width);
    _shader.setUniform("color", color);
    _shader.setUniform("useVertexColor", useVertexColor);
    _shader.setUniform("zOffset", zOffset);
    vao->draw();
}

}
} // namespace Lattice
