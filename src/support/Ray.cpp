#include "Ray.hpp"

using namespace glm;

namespace Lattice {

RayPointDistance::RayPointDistance(const Ray &l, const dvec3 p) {
    // http://geomalgorithms.com/a02-_lines.html#Distance-to-Infinite-Line
    dvec3 w = p - l.origin;
    dvec3 v = glm::normalize(l.direction);
    distance = length(cross(v, w));
    t = dot(v, w);
}

RayRayDistance::RayRayDistance(const Ray &l0, const Ray &l1) {
    // http://geomalgorithms.com/a07-_distance.html
    dvec3 u = l0.direction;
    dvec3 v = l1.direction;
    dvec3 w0 = l0.origin - l1.origin;

    double a = dot(u, u);
    double b = dot(u, v);
    double c = dot(v, v);
    double d = dot(u, w0);
    double e = dot(v, w0);

    double denom = a * c - b * b;

    t0 = (b * e - c * d) / denom;
    t1 = (a * e - b * d) / denom;

    dvec3 p = l0.origin + l0.direction * t0;
    dvec3 q = l1.origin + l1.direction * t1;

    distance = glm::distance(p, q);
}

} // namespace Lattice
