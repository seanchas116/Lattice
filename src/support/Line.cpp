#include "Line.hpp"

using namespace glm;

namespace Lattice {

LinePointDistance::LinePointDistance(const Line &l, const glm::vec3 p) {
    glm::vec3 w = p - l.a;
    glm::vec3 v = glm::normalize(l.b - l.a);
    distance = length(cross(v, w));
    t = dot(v, w);
}

LineLineDistance::LineLineDistance(const Line &l0, const Line &l1) {
    vec3 u = l0.b - l0.a;
    vec3 v = l1.b - l1.a;
    vec3 w0 = l0.a - l1.a;

    float a = dot(u, u);
    float b = dot(u, v);
    float c = dot(v, v);
    float d = dot(u, w0);
    float e = dot(v, w0);

    float denom = a * c - b * b;

    t0 = (b * e - c * d) / denom;
    t1 = (a * e - b * d) / denom;

    vec3 p = mix(l0.a, l0.b, t0);
    vec3 q = mix(l1.a, l1.b, t1);

    distance = glm::distance(p, q);
}

} // namespace Lattice
