#pragma once
#include "../support/Shorthands.hpp"
#include <glm/glm.hpp>

namespace Lattice {
namespace Viewport {

class Renderable;

struct HitResult {
    SP<Renderable> renderable;
    float depth;
};

} // namespace Viewport
} // namespace Lattice
