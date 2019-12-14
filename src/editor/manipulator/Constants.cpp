#include "Constants.hpp"

namespace Lattice {
namespace Editor {
namespace Manipulator {
namespace Constants {

// TODO: specify better colors with HSV
const std::array<glm::vec4, 3> colors{
    glm::vec4(0.9, 0, 0, 1),
    glm::vec4(0, 0.9, 0, 1),
    glm::vec4(0, 0, 0.9, 1),
};

const std::array<glm::vec4, 3> hoverColors{
    glm::vec4(1, 0.1, 0.1, 1),
    glm::vec4(0.1, 1, 0.1, 1),
    glm::vec4(0.1, 0.1, 1, 1),
};
} // namespace Constants

} // namespace Manipulator
} // namespace Editor
} // namespace Lattice
