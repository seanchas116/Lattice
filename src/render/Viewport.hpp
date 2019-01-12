#pragma once

#include "../support/Camera.hpp"

namespace Lattice {
namespace Render {

struct Viewport {
    glm::ivec2 offset;
    Camera camera;
};

} // namespace Renderer
} // namespace Lattice
