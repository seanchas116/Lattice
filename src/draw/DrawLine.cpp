#include "DrawLine.hpp"
#include "../support/Camera.hpp"
#include "../gl/VAO.hpp"
#include "../resource/Resource.hpp"
#include <glm/gtx/transform.hpp>

using namespace glm;

namespace Lattice {
namespace Draw {

DrawLine::DrawLine() : _shader(Resource::read("src/draw/DrawLine.vert"),
                               Resource::read("src/draw/DrawLine.geom"),
                               Resource::read("src/draw/DrawLine.frag")),
                       _shader2D(Resource::read("src/draw/DrawLine2D.vert"),
                                 Resource::read("src/draw/DrawLine2D.geom"),
                                 Resource::read("src/draw/DrawLine.frag")) {
}

void DrawLine::draw(const SP<GL::VAO> &vao, const glm::dmat4 &matrix, const SP<Camera> &camera, double width, glm::vec4 color, bool useVertexColor, double zOffset) {
    _shader.bind();
    _shader.setUniform("MV", camera->worldToCameraMatrix() * matrix);
    _shader.setUniform("P", camera->cameraToViewportMatrix());
    _shader.setUniform("viewportSize", camera->viewportSize());
    _shader.setUniform("zNear", camera->projection() == Camera::Projection::Perspective ? camera->zNear() : -10000.0); // TODO: specify depth in better way
    _shader.setUniform("width", width);
    _shader.setUniform("color", color);
    _shader.setUniform("useVertexColor", useVertexColor);
    _shader.setUniform("zOffset", zOffset);
    vao->draw();
}

void DrawLine::draw2D(const SP<GL::VAO> &vao, const glm::dmat4 &matrix, glm::ivec2 viewportSize, double width, glm::vec4 color, bool useVertexColor) {
    _shader.bind();
    dmat4 MVP = translate(dvec3(-1.0)) * scale(dvec3(2.0 / dvec2(viewportSize), 2.0)) * matrix;
    _shader.setUniform("MVP", MVP);
    _shader.setUniform("viewportSize", dvec2(viewportSize));
    _shader.setUniform("width", width);
    _shader.setUniform("color", color);
    _shader.setUniform("useVertexColor", useVertexColor);
    vao->draw();
}

}
} // namespace Lattice
