#include "Constants.hpp"

namespace Lattice {
namespace Editor {
namespace Manipulator {
namespace Constants {

    // TODO: specify better colors with HSV
    const std::array<glm::dvec3, 3> colors {
        glm::dvec3(0.9, 0, 0),
        glm::dvec3(0, 0.9, 0),
        glm::dvec3(0, 0, 0.9),
    };

    const std::array<glm::dvec3, 3> hoverColors {
        glm::dvec3(1, 0.1, 0.1),
        glm::dvec3(0.1, 1, 0.1),
        glm::dvec3(0.1, 0.1, 1),
    };
}

}
} // namespace Editor
} // namespace Lattice
