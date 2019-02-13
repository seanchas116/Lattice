#include "DrawPickableID.hpp"
#include "../Pickable.hpp"
#include "../../gl/VAO.hpp"
#include "../../resource/Resource.hpp"
#include "../../support/Camera.hpp"

namespace Lattice::Render {

DrawPickableID::DrawPickableID() :
    _shader(Resource::read("src/render/operations/DrawPickableID.vert"), std::string(), Resource::read("src/render/operations/DrawPickableID.frag"))
{
}

void DrawPickableID::draw(const SP<GL::VAO> &vao, const glm::dmat4 &matrix, const SP<Camera> &camera, const SP<Pickable> &pickable) {
    _shader.bind();
    _shader.setUniform("pickableID", glm::ivec4(pickable->toIDColor()));
    _shader.setUniform("MVP", camera->worldToScreenMatrix() * matrix);
    vao->draw();
}

} // namespace Lattice
