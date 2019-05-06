#pragma once
#include "../Mesh.hpp"

namespace Lattice {
namespace Mesh {

class AddSphere {
public:
    AddSphere(glm::vec3 center, float radius, int segmentCount, int ringCount, int axis, uint32_t material) : center(center),
                                                                                                              radius(radius),
                                                                                                              segmentCount(segmentCount),
                                                                                                              ringCount(ringCount),
                                                                                                              axis(axis),
                                                                                                              material(material) {}

    Mesh perform(const Mesh& original);

    glm::vec3 center;
    float radius;
    int segmentCount;
    int ringCount;
    int axis;
    uint32_t material;
};

} // namespace Mesh
} // namespace Lattice

