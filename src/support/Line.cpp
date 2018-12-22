#include "Line.hpp"

namespace Lattice {

std::tuple<float, float> Line::distance(glm::vec3 p) const {
    glm::vec3 w = p - a;
    float d = length(cross(direction, w));
    float t = dot(direction, w);
    return std::tuple(d, t);
}

} // namespace Lattice
