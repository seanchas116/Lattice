#include "DrawUnicolor.hpp"
#include "../gl/VAO.hpp"
#include "../resource/Resource.hpp"
#include "../support/OldCamera.hpp"

namespace Lattice {
namespace Draw {

DrawUnicolor::DrawUnicolor() : _shader(Resource::read("src/draw/DrawUnicolor.vert"),
                                       std::string(),
                                       Resource::read("src/draw/DrawUnicolor.frag")) {
}

void DrawUnicolor::draw(const SP<GL::VAO> &vao, const glm::dmat4 &matrix, const SP<OldCamera> &camera, glm::vec4 color, bool useVertexColor) {
    _shader.bind();
    _shader.setUniform("color", color);
    _shader.setUniform("useVertexColor", useVertexColor);
    _shader.setUniform("MVP", camera->worldToViewportMatrix() * matrix);
    vao->draw();
}

}
} // namespace Lattice
