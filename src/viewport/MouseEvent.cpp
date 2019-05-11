#include "MouseEvent.hpp"

namespace Lattice {
namespace Viewport {

glm::dvec3 MouseEvent::worldPos() const {
    return camera->mapViewportToWorld(viewportPos);
}

}
}
