#include "PointsDrawable.hpp"
#include "../resource/Resource.hpp"
#include "../gl/Shader.hpp"

namespace Lattice {
namespace Drawable {

PointsDrawable::PointsDrawable() :
    _shader(makeShared<GL::Shader>(Resource::read("src/drawable/Points.vert"), Resource::read("src/drawable/Points.geom"), Resource::read("src/drawable/Points.frag")))
{
}

} // namespace Drawable
} // namespace Lattice
