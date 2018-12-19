#pragma once

#include <glm/glm.hpp>
#include <tuple>

namespace Lattice {

class Line {
public:
    Line(glm::vec3 a, glm::vec3 b) : a(a), b(b), direction(glm::normalize(b - a)) {}

    std::tuple<float, float> distance(glm::vec3 p) const;

    const glm::vec3 a;
    const glm::vec3 b;
    const glm::vec3 direction;
};

} // namespace Lattice
