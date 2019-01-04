#include "Ray.hpp"

using namespace glm;

namespace Lattice {

RayPointDistance::RayPointDistance(const Ray &l, const dvec3 p) {
    // http://geomalgorithms.com/a02-_lines.html#Distance-to-Infinite-Line
    dvec3 w = p - l.a;
    dvec3 v = glm::normalize(l.b - l.a);
    distance = length(cross(v, w));
    t = dot(v, w);
}

RayRayDistance::RayRayDistance(const Ray &l0, const Ray &l1) {
    // http://geomalgorithms.com/a07-_distance.html
    dvec3 u = l0.b - l0.a;
    dvec3 v = l1.b - l1.a;
    dvec3 w0 = l0.a - l1.a;

    double a = dot(u, u);
    double b = dot(u, v);
    double c = dot(v, v);
    double d = dot(u, w0);
    double e = dot(v, w0);

    double denom = a * c - b * b;

    t0 = (b * e - c * d) / denom;
    t1 = (a * e - b * d) / denom;

    dvec3 p = mix(l0.a, l0.b, t0);
    dvec3 q = mix(l1.a, l1.b, t1);

    distance = glm::distance(p, q);
}

} // namespace Lattice
