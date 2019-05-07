#pragma once
#include "../Mesh.hpp"

namespace Lattice {
namespace Mesh {

class BuildCylinder {
public:
    BuildCylinder(glm::vec3 center, float radius, float height, int segmentCount, int axis, uint32_t material) : center(center),
                                                                                                           radius(radius),
                                                                                                           height(height),
                                                                                                           segmentCount(segmentCount),
                                                                                                           axis(axis),
                                                                                                           material(material) {}

    Mesh perform() const;

    glm::vec3 center;
    float radius;
    float height;
    int segmentCount;
    int axis;
    uint32_t material;
};

} // namespace Mesh
} // namespace Lattice

