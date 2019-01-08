#pragma once

#include "../support/Camera.hpp"

namespace Lattice {
namespace Renderer {

struct Viewport {
    Camera camera;
    glm::ivec2 offset;
};

} // namespace Renderer
} // namespace Lattice
