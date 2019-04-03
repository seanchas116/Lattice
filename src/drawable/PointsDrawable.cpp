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

const SP<GL::Shader> &PointsDrawable::shader() {
    // TODO: support context loss
    auto shader = makeShared<GL::Shader>(Resource::read("src/drawable/Points.vert"), Resource::read("src/drawable/Points.geom"), Resource::read("src/drawable/Points.frag"));
    return shader;
}

} // namespace Drawable
} // namespace Lattice
