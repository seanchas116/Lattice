#pragma once
#include "../Mesh.hpp"

namespace Lattice {
namespace Mesh {

class BuildCone {
public:
    BuildCone(glm::vec3 center, float radius, float height, int segmentCount, int axis, MaterialHandle material) : center(center),
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
    MaterialHandle material;
};

} // namespace Mesh
} // namespace Lattice

