#include "DrawMaterial.hpp"
#include "../../gl/VAO.hpp"
#include "../../resource/Resource.hpp"
#include "../../support/Projection.hpp"

namespace Lattice {

DrawMaterial::DrawMaterial() :
    _shader(readResource("src/viewport/operations/DrawMaterial.vert"), std::string(), readResource("src/viewport/operations/DrawMaterial.frag"))
{
}

void DrawMaterial::draw(const SP<VAO> &vao, const glm::mat4 &matrix, const Projection &projection, const SP<MeshMaterial> &material) {
    _shader.bind();
    _shader.setUniform("diffuse", material->baseColor());
    _shader.setUniform("ambient", glm::vec3(0));
    _shader.setUniform("MV", matrix);
    _shader.setUniform("MVP", projection.matrix() * matrix);
    vao->draw();
}

} // namespace Lattice
