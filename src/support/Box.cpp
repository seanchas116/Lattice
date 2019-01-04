#include "Box.hpp"
#include "Ray.hpp"
#include <QtGlobal>

using namespace glm;

namespace Lattice {

bool Box::intersects(const Ray &ray) const {
    // https://gamedev.stackexchange.com/a/18459

    dvec3 dirfrac = 1.0 / ray.direction;

    double t1 = (minPosition.x - ray.origin.x)*dirfrac.x;
    double t2 = (maxPosition.x - ray.origin.x)*dirfrac.x;
    double t3 = (minPosition.y - ray.origin.y)*dirfrac.y;
    double t4 = (maxPosition.y - ray.origin.y)*dirfrac.y;
    double t5 = (minPosition.z - ray.origin.z)*dirfrac.z;
    double t6 = (maxPosition.z - ray.origin.z)*dirfrac.z;

    double tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
    double tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

    return 0 <= tmax && tmin <= tmax;
}

} // namespace Lattice
