#pragma once
#include "../Mesh.hpp"

namespace Lattice {
namespace Mesh {

class BuildCircle {
public:
    // TODO: support multiple fill types
    BuildCircle(glm::vec3 center, float radius, int segmentCount, int normalAxis, MaterialHandle material) : center(center),
                                                                                                     radius(radius),
                                                                                                     segmentCount(segmentCount),
                                                                                                     normalAxis(normalAxis),
                                                                                                     material(material) {}

    Mesh perform() const;

    glm::vec3 center;
    float radius;
    int segmentCount;
    int normalAxis;
    MaterialHandle material;
};

} // namespace Mesh
} // namespace Lattice

