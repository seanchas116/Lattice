#include "DrawSolid.hpp"
#include "../../gl/VAO.hpp"
#include "../../resource/Resource.hpp"
#include "../../support/Projection.hpp"

namespace Lattice {

DrawSolid::DrawSolid() :
    _shader(readResource("src/viewport/shaders/DrawSolid.vert"), std::string(), readResource("src/viewport/shaders/DrawSolid.frag"))
{
}

void DrawSolid::draw(const SP<VAO> &vao, const glm::mat4 &matrix, const Projection &projection, glm::vec3 diffuse, glm::vec3 ambient) {
    _shader.bind();
    _shader.setUniform("diffuse", diffuse);
    _shader.setUniform("ambient", ambient);
    _shader.setUniform("MV", matrix);
    _shader.setUniform("MVP", projection.matrix() * matrix);
    vao->draw();
}

} // namespace Lattice
