#include "PointsDrawable.hpp"
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

const SP<GL::Shader> &PointsDrawable::shader() {
    // TODO: support context loss
    static auto shader = makeShared<GL::Shader>(Resource::read("src/drawable/Points.vert"), Resource::read("src/drawable/Points.geom"), Resource::read("src/drawable/Points.frag"));
    return shader;
}

} // namespace Drawable
} // namespace Lattice
