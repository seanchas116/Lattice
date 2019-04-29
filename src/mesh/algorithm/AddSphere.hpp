#pragma once
#include "../Mesh.hpp"

namespace Lattice {
namespace Mesh {

class AddSphere {
public:
    AddSphere(glm::vec3 center, float radius, int segmentCount, int ringCount, int axis, uint32_t material) : center(center),
                                                                                                              radius(radius),
                                                                                                              segmentCount(segmentCount),
                                                                                                              ringCount(ringCount),
                                                                                                              axis(axis),
                                                                                                              material(material) {}

    void redo(Mesh& mesh);
    void undo(Mesh& mesh);

    glm::vec3 center;
    float radius;
    int segmentCount;
    int ringCount;
    int axis;
    uint32_t material;
    std::vector<VertexHandle> vertices;
};

} // namespace Mesh
} // namespace Lattice

