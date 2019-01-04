#pragma once
#include <glm/glm.hpp>

namespace Lattice {

class Ray;

class Box {
public:
    Box() = default;
    Box(glm::dvec3 min, glm::dvec3 max) : minPosition(min), maxPosition(max) {}

    bool intersects(const Ray& ray) const;

    glm::dvec3 minPosition {0};
    glm::dvec3 maxPosition {0};
};

} // namespace Lattice
