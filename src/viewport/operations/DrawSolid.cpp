#include "DrawSolid.hpp"
#include "../../gl/VAO.hpp"
#include "../../resource/Resource.hpp"
#include "../../support/Camera.hpp"

namespace Lattice {

DrawSolid::DrawSolid() :
    _shader(readResource("src/viewport/operations/DrawSolid.vert"), std::string(), readResource("src/viewport/operations/DrawSolid.frag"))
{
}

void DrawSolid::draw(const SP<VAO> &vao, const glm::mat4 &matrix, const Camera &camera, glm::vec3 diffuse, glm::vec3 ambient) {
    _shader.bind();
    _shader.setUniform("diffuse", diffuse);
    _shader.setUniform("ambient", ambient);
    _shader.setUniform("MV", camera.worldToCameraMatrix() * matrix);
    _shader.setUniform("MVP", camera.worldToScreenMatrix() * matrix);
    vao->draw();
}

} // namespace Lattice
