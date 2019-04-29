#pragma once
#include "../Mesh.hpp"

namespace Lattice {
namespace Mesh {

class AddCube {
public:
    AddCube(glm::vec3 minPos, glm::vec3 maxPos) : _minPos(minPos), _maxPos(maxPos) {}

    void perform(Mesh& mesh);

private:
    glm::vec3 _minPos;
    glm::vec3 _maxPos;
};

} // namespace Mesh
} // namespace Lattice

