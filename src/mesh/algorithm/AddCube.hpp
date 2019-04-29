#pragma once
#include "../Mesh.hpp"

namespace Lattice {
namespace Mesh {

class AddCube {
public:
    AddCube(glm::vec3 minPos, glm::vec3 maxPos, uint32_t material) : minPos(minPos), maxPos(maxPos), material(material) {}

    void redo(Mesh& mesh);
    void undo(Mesh& mesh);

    glm::vec3 minPos;
    glm::vec3 maxPos;
    uint32_t material;
    std::vector<VertexHandle> vertices;
};

} // namespace Mesh
} // namespace Lattice

