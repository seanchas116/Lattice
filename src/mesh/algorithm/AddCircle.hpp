#pragma once
#include "../Mesh.hpp"

namespace Lattice {
namespace Mesh {

class AddCircle {
public:
    // TODO: support multiple fill types
    AddCircle(glm::vec3 center, float radius, int segmentCount, int normalAxis, uint32_t material) : center(center),
                                                                                                     radius(radius),
                                                                                                     segmentCount(segmentCount),
                                                                                                     normalAxis(normalAxis),
                                                                                                     material(material) {}

    Mesh perform(const Mesh& mesh);

    glm::vec3 center;
    float radius;
    int segmentCount;
    int normalAxis;
    uint32_t material;
};

} // namespace Mesh
} // namespace Lattice

