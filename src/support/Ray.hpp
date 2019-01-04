#pragma once

#include <glm/glm.hpp>
#include <tuple>

namespace Lattice {

class Ray final {
public:
    Ray() : a(0), b(0) {}
    Ray(glm::dvec3 a, glm::dvec3 b) : a(a), b(b) {}

    glm::dvec3 a;
    glm::dvec3 b;
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
