#include "LinesDrawable.hpp"
#include "../support/Camera.hpp"
#include "../resource/Resource.hpp"
#include "../gl/Shader.hpp"
#include "../gl/VAO.hpp"
#include "../gl/IndexBuffer.hpp"
#include "../gl/VertexBuffer.hpp"
#include <glm/gtx/transform.hpp>

namespace Lattice {
namespace Drawable {

LinesDrawable::LinesDrawable() :
    _vbo(makeShared<GL::VertexBuffer<Point>>()),
    _ibo(makeShared<GL::IndexBuffer>()),
    _vao(makeShared<GL::VAO>(_vbo, _ibo))
{
}

void LinesDrawable::setPoints(const std::vector<Point> &points) {
    _vbo->setVertices(points);
}

void LinesDrawable::setIndices(const std::vector<std::array<uint32_t, 2>> &indices) {
    _ibo->setLines(indices);
}

void LinesDrawable::draw(const glm::dmat4 &matrix, const SP<Camera> &camera) {
    shader()->bind();
    shader()->setUniform("MV", camera->worldToCameraMatrix() * matrix);
    shader()->setUniform("P", camera->cameraToViewportMatrix());
    shader()->setUniform("viewportSize", camera->viewportSize());
    shader()->setUniform("zNear", camera->projection() == Camera::Projection::Perspective ? camera->zNear() : -10000.0); // TODO: specify depth in better way
    shader()->setUniform("width", _width);
    shader()->setUniform("color", glm::vec3(0));
    shader()->setUniform("useVertexColor", true);
    shader()->setUniform("zOffset", _zOffset);
    _vao->draw();
}

const SP<GL::Shader> &LinesDrawable::shader() {
    // TODO: support context loss
    static auto shader = makeShared<GL::Shader>(
        Resource::read("src/drawable/LinesDrawable.vert"),
        Resource::read("src/drawable/LinesDrawable.geom"),
        Resource::read("src/drawable/LinesDrawable.frag"));
    return shader;
}

} // namespace Drawable
} // namespace Lattice
