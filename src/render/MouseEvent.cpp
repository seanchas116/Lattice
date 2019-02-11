#include "MouseEvent.hpp"

namespace Lattice {
namespace Render {

glm::dvec3 MouseEvent::worldPos() const {
    return camera->mapScreenToWorld(glm::dvec3(screenPos, hitResult.depth));
}

}
}
