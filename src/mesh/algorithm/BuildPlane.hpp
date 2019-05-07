#pragma once
#include "../Mesh.hpp"

namespace Lattice {
namespace Mesh {

class BuildPlane {
public:
    BuildPlane(glm::vec3 center, glm::vec2 size, int normalAxis, uint32_t material) :center(center),
                                                                                    size(size),
                                                                                    normalAxis(normalAxis),
                                                                                    material(material) {}

    Mesh perform() const;

    glm::vec3 center;
    glm::vec2 size;
    int normalAxis;
    uint32_t material;
};

} // namespace Mesh
} // namespace Lattice

