#include "DrawSolid.hpp"
#include "../gl/VAO.hpp"
#include "../resource/Resource.hpp"
#include "../support/Camera.hpp"

namespace Lattice {
namespace Draw {

DrawSolid::DrawSolid() : _shader(Resource::read("src/draw/DrawSolid.vert"),
                                 std::string(),
                                 Resource::read("src/draw/DrawSolid.frag")) {
}

void DrawSolid::draw(const SP<GL::VAO> &vao, const glm::dmat4 &matrix, const SP<Camera> &camera, glm::dvec3 diffuse, glm::dvec3 ambient) {
    _shader.bind();
    _shader.setUniform("diffuse", diffuse);
    _shader.setUniform("ambient", ambient);
    _shader.setUniform("MV", camera->worldToCameraMatrix() * matrix);
    _shader.setUniform("MVP", camera->worldToViewportMatrix() * matrix);
    vao->draw();
}

}
} // namespace Lattice
