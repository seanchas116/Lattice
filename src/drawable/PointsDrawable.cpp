#include "PointsDrawable.hpp"
#include "../support/Camera.hpp"
#include "../resource/Resource.hpp"
#include "../gl/Shader.hpp"
#include "../gl/VAO.hpp"
#include "../gl/VertexBuffer.hpp"

namespace Lattice {
namespace Drawable {

PointsDrawable::PointsDrawable() :
    _vbo(makeShared<GL::VertexBuffer<Point>>()),
    _vao(makeShared<GL::VAO>(_vbo, GL::Primitive::Point))
{
}

void PointsDrawable::setPoints(const std::vector<Point> &points) {
    _vbo->setVertices(points);
}

void PointsDrawable::draw(const glm::dmat4 &matrix, const SP<Camera> &camera) {
    shader()->bind();
    shader()->setUniform("MVP", camera->worldToViewportMatrix() * matrix);
    shader()->setUniform("viewportSize", camera->viewportSize());
    shader()->setUniform("width", _width);
    shader()->setUniform("color", glm::vec3(0));
    shader()->setUniform("useVertexColor", true);
    shader()->setUniform("zOffset", _zOffset);
    _vao->draw();
}

const SP<GL::Shader> &PointsDrawable::shader() {
    // TODO: support context loss
    static auto shader = makeShared<GL::Shader>(Resource::read("src/drawable/Points.vert"), Resource::read("src/drawable/Points.geom"), Resource::read("src/drawable/Points.frag"));
    return shader;
}

} // namespace Drawable
} // namespace Lattice
