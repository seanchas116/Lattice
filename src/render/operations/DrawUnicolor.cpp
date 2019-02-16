#include "DrawUnicolor.hpp"
#include "../Renderable.hpp"
#include "../../gl/VAO.hpp"
#include "../../resource/Resource.hpp"
#include "../../support/Camera.hpp"

namespace Lattice::Render {

DrawUnicolor::DrawUnicolor() :
    _shader(Resource::read("src/render/operations/DrawUnicolor.vert"), std::string(), Resource::read("src/render/operations/DrawUnicolor.frag"))
{
}

void DrawUnicolor::draw(const SP<GL::VAO> &vao, const glm::dmat4 &matrix, const SP<Camera> &camera, glm::vec4 color, bool useVertexColor) {
    _shader.bind();
    _shader.setUniform("color", color);
    _shader.setUniform("useVertexColor", useVertexColor);
    _shader.setUniform("MVP", camera->worldToScreenMatrix() * matrix);
    vao->draw();
}

} // namespace Lattice
