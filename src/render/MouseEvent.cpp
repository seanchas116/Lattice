#include "MouseEvent.hpp"

namespace Lattice {
namespace Render {

LocatedEvent::~LocatedEvent() {
}

glm::dvec3 LocatedEvent::worldPos() const {
    return camera->mapViewportToWorld(viewportPos);
}

}
}
