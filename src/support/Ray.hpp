#pragma once

#include <glm/glm.hpp>
#include <tuple>

namespace Lattice {

class Ray final {
public:
    Ray() = default;
    Ray(glm::dvec3 origin, glm::dvec3 direction) : origin(origin), direction(direction) {}

    glm::dvec3 origin {0};
    glm::dvec3 direction {0};
};

class RayPointDistance final {
public:
    RayPointDistance(const Ray& l, const glm::dvec3 p);
    double t;
    double distance;
};

class RayRayDistance final {
public:
    RayRayDistance(const Ray& l0, const Ray& l1);
    double t0;
    double t1;
    double distance;
};

} // namespace Lattice
