#include "PointsDrawable.hpp"
#include "../resource/Resource.hpp"
#include "../gl/Shader.hpp"

namespace Lattice {
namespace Drawable {

PointsDrawable::PointsDrawable() {
}

const SP<GL::Shader> &PointsDrawable::shader() {
    // TODO: support context loss
    auto shader = makeShared<GL::Shader>(Resource::read("src/drawable/Points.vert"), Resource::read("src/drawable/Points.geom"), Resource::read("src/drawable/Points.frag"));
    return shader;
}

} // namespace Drawable
} // namespace Lattice
