#pragma once
#include <glm/glm.hpp>
#include "Ray.hpp"

namespace Lattice {

template <typename T>
class Box {
public:
    Box() = default;
    Box(glm::tvec3<T> min, glm::tvec3<T> max) : minPosition(min), maxPosition(max) {}

    bool intersects(const Ray<T>& ray) const {
        // https://gamedev.stackexchange.com/a/18459
        using namespace glm;

        tvec3<T> dirfrac = tvec3<T>(1) / ray.direction;

        T t1 = (minPosition.x - ray.origin.x)*dirfrac.x;
        T t2 = (maxPosition.x - ray.origin.x)*dirfrac.x;
        T t3 = (minPosition.y - ray.origin.y)*dirfrac.y;
        T t4 = (maxPosition.y - ray.origin.y)*dirfrac.y;
        T t5 = (minPosition.z - ray.origin.z)*dirfrac.z;
        T t6 = (maxPosition.z - ray.origin.z)*dirfrac.z;

        T tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
        T tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

        return 0 <= tmax && tmin <= tmax;
    }

    glm::tvec3<T> minPosition {0};
    glm::tvec3<T> maxPosition {0};
};

} // namespace Lattice
