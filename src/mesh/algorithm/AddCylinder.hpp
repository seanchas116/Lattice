#pragma once
#include "../Mesh.hpp"

namespace Lattice {
namespace Mesh {

class AddCylinder {
public:
    AddCylinder(glm::vec3 center, float radius, float height, int segmentCount, int axis, uint32_t material) : center(center),
                                                                                                           radius(radius),
                                                                                                           height(height),
                                                                                                           segmentCount(segmentCount),
                                                                                                           axis(axis),
                                                                                                           material(material) {}

    void redo(Mesh& mesh);
    void undo(Mesh& mesh);

    glm::vec3 center;
    float radius;
    float height;
    int segmentCount;
    int axis;
    uint32_t material;
    std::vector<VertexHandle> vertices;
};

} // namespace Mesh
} // namespace Lattice

