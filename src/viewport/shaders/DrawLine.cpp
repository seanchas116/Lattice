#include "DrawLine.hpp"
#include "../../support/Projection.hpp"
#include "../../gl/LineVAO.hpp"
#include "../../resource/Resource.hpp"

namespace Lattice {

DrawLine::DrawLine() :
    _shader(readResource("src/viewport/shaders/ThickLineShader.vert"), readResource("src/viewport/shaders/ThickLineShader.geom"), readResource("src/viewport/shaders/ThickLineShader.frag"))
{
}

void DrawLine::draw(const SP<LineVAO> &vao, const glm::mat4 &matrix, const Projection &projection, float width, glm::vec3 color)
{
    _shader.setUniform("MV", matrix);
    _shader.setUniform("P", projection.matrix());
    _shader.setUniform("viewportSize", projection.viewSize());
    _shader.setUniform("width", width);
    _shader.setUniform("color", color);
    vao->draw();
}

} // namespace Lattice
