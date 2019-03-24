#include "MouseEvent.hpp"

namespace Lattice {
namespace Render {

glm::dvec3 MouseEvent::worldPos() const {
    return camera->mapViewportToWorld(glm::dvec3(viewportPos, depth));
}

}
}
