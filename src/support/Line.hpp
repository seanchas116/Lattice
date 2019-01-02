#pragma once

#include <glm/glm.hpp>
#include <tuple>

namespace Lattice {

class Line {
public:
    Line() : a(0), b(0) {}
    Line(glm::dvec3 a, glm::dvec3 b) : a(a), b(b) {}

    glm::dvec3 a;
    glm::dvec3 b;
};

class LinePointDistance {
public:
    LinePointDistance(const Line& l, const glm::dvec3 p);
    double t;
    double distance;
};

class LineLineDistance {
public:
    LineLineDistance(const Line& l0, const Line& l1);
    double t0;
    double t1;
    double distance;
};

} // namespace Lattice
