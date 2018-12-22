#pragma once

#include <glm/glm.hpp>
#include <tuple>

namespace Lattice {

class Line {
public:
    Line(glm::vec3 a, glm::vec3 b) : a(a), b(b) {}

    const glm::vec3 a;
    const glm::vec3 b;
};

class LinePointDistance {
public:
    LinePointDistance(const Line& l, const glm::vec3 p);
    float t;
    float distance;
};

class LineLineDistance {
public:
    LineLineDistance(const Line& l0, const Line& l1);
    float t0;
    float t1;
    float distance;
};

} // namespace Lattice
