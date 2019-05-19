#pragma once
#include "../Mesh.hpp"

namespace Lattice {
namespace Mesh {

class BuildSphere {
public:
    BuildSphere(glm::vec3 center, float radius, int segmentCount, int ringCount, int axis, MaterialHandle material) : center(center),
                                                                                                              radius(radius),
                                                                                                              segmentCount(segmentCount),
                                                                                                              ringCount(ringCount),
                                                                                                              axis(axis),
                                                                                                              material(material) {}

    Mesh perform() const;

    glm::vec3 center;
    float radius;
    int segmentCount;
    int ringCount;
    int axis;
    MaterialHandle material;
};

} // namespace Mesh
} // namespace Lattice

