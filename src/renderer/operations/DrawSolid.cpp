#include "DrawSolid.hpp"
#include "../../gl/VAO.hpp"
#include "../../resource/Resource.hpp"
#include "../../support/Camera.hpp"

namespace Lattice::Renderer {

DrawSolid::DrawSolid() :
    _shader(Resource::read("src/renderer/operations/DrawSolid.vert"), std::string(), Resource::read("src/renderer/operations/DrawSolid.frag"))
{
}

void DrawSolid::draw(const SP<GL::VAO> &vao, const glm::dmat4 &matrix, const Camera &camera, glm::dvec3 diffuse, glm::dvec3 ambient) {
    _shader.bind();
    _shader.setUniform("diffuse", diffuse);
    _shader.setUniform("ambient", ambient);
    _shader.setUniform("MV", camera.worldToCameraMatrix() * matrix);
    _shader.setUniform("MVP", camera.worldToScreenMatrix() * matrix);
    vao->draw();
}

} // namespace Lattice
