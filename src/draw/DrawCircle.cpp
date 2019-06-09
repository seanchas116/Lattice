#include "DrawCircle.hpp"
#include "../gl/VAO.hpp"
#include "../resource/Resource.hpp"
#include "../support/Location.hpp"
#include "../support/Camera.hpp"
#include <glm/gtx/transform.hpp>

using namespace glm;

namespace Lattice {
namespace Draw {

DrawCircle::DrawCircle() : _shader(Resource::read("src/draw/DrawCircle.vert"),
                                   Resource::read("src/draw/DrawCircle.geom"),
                                   Resource::read("src/draw/DrawCircle.frag")) {
}

void DrawCircle::draw(const SP<GL::VAO> &vao, const glm::dmat4 &matrix, const Camera &camera, double width, glm::vec4 color, bool useVertexColor, double zOffset) {
    _shader.bind();
    _shader.setUniform("MVP", camera.worldToViewportMatrix() * matrix);
    _shader.setUniform("viewportSize", camera.viewportSize());
    _shader.setUniform("width", width);
    _shader.setUniform("color", color);
    _shader.setUniform("useVertexColor", useVertexColor);
    _shader.setUniform("zOffset", zOffset);
    vao->draw();
}

void DrawCircle::draw2D(const SP<GL::VAO> &vao, const glm::dmat4 &matrix, glm::ivec2 viewportSize, double width, glm::vec4 color, bool useVertexColor) {
    _shader.bind();
    dmat4 MVP = translate(dvec3(-1.0)) * scale(dvec3(2.0 / dvec2(viewportSize), 2.0)) * matrix;
    _shader.setUniform("MVP", MVP);
    _shader.setUniform("viewportSize", dvec2(viewportSize));
    _shader.setUniform("width", width);
    _shader.setUniform("color", color);
    _shader.setUniform("useVertexColor", useVertexColor);
    _shader.setUniform("zOffset", 0.0);
    vao->draw();
}

}
} // namespace Lattice
