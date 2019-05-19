#pragma once
#include "../Mesh.hpp"

namespace Lattice {
namespace Mesh {

class BuildCube {
public:
    BuildCube(glm::vec3 minPos, glm::vec3 maxPos, MaterialHandle material) : minPos(minPos),
                                                                     maxPos(maxPos),
                                                                     material(material) {}

    Mesh perform() const;

    glm::vec3 minPos;
    glm::vec3 maxPos;
    MaterialHandle material;
};

} // namespace Mesh
} // namespace Lattice

