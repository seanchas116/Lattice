#pragma once
#include "../Mesh.hpp"

namespace Lattice {
namespace Mesh {

class BuildCube {
public:
    BuildCube(glm::vec3 minPos, glm::vec3 maxPos, uint32_t material) : minPos(minPos),
                                                                     maxPos(maxPos),
                                                                     material(material) {}

    Mesh perform() const;

    glm::vec3 minPos;
    glm::vec3 maxPos;
    uint32_t material;
};

} // namespace Mesh
} // namespace Lattice

