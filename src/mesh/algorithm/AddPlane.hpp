#pragma once
#include "../Mesh.hpp"

namespace Lattice {
namespace Mesh {

class AddPlane {
public:
    AddPlane(glm::vec3 center, glm::vec2 size, int normalAxis, uint32_t material) : center(center), size(size), normalAxis(normalAxis), material(material) {}

    void redo(Mesh& mesh);
    void undo(Mesh& mesh);

    glm::vec3 center;
    glm::vec2 size;
    int normalAxis;
    uint32_t material;
    std::vector<VertexHandle> vertices;
};

} // namespace Mesh
} // namespace Lattice

