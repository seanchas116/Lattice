#pragma once
#include "../Mesh.hpp"

namespace Lattice {
namespace Mesh {

class AddCube {
public:
    AddCube(glm::vec3 minPos, glm::vec3 maxPos, uint32_t material) : _minPos(minPos), _maxPos(maxPos), _material(material) {}

    void perform(Mesh& mesh);

private:
    glm::vec3 _minPos;
    glm::vec3 _maxPos;
    uint32_t _material;
};

} // namespace Mesh
} // namespace Lattice

