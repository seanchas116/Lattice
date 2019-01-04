#pragma once
#include <glm/glm.hpp>

namespace Lattice {

class Line;

class Box {
public:
    Box() = default;
    Box(glm::dvec3 min, glm::dvec3 max) : min(min), max(max) {}

    bool intersects(const Line& line) const;

    glm::dvec3 min {0};
    glm::dvec3 max {0};
};

} // namespace Lattice
