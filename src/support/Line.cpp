#include "Line.hpp"

namespace Lattice {

std::tuple<float, float> Line::distance(glm::vec3 p) const {
    float t = glm::dot(p - a, direction);
    glm::vec3 q = a + t * direction;
    return std::tuple(glm::distance(p, q), t);
}

} // namespace Lattice
