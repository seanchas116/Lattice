#include "AddCircle.hpp"
#include <QtGlobal>

using namespace glm;

namespace Lattice {
namespace Mesh {

void AddCircle::redo(Mesh &mesh) {
    vertices.clear();

    std::vector<UVPointHandle> uvPoints;

    float angleStep = float(M_PI) * 2.f / segmentCount;
    for (int i = 0 ; i < segmentCount; ++i) {
        float angle = angleStep * i;
        vec3 offset(0);
        offset[(normalAxis + 1) % 3] = cos(angle);
        offset[(normalAxis + 2) % 3] = sin(angle);
        vec3 pos = center + offset * radius;
        auto v = mesh.addVertex(pos);
        vertices.push_back(v);
        auto uv = mesh.addUVPoint(v, vec2(0));
        uvPoints.push_back(uv);
    }

    mesh.addFace(uvPoints, material);
}

void AddCircle::undo(Mesh &mesh) {
    for (auto v : vertices) {
        mesh.removeVertex(v);
    }
}

} // namespace Mesh
} // namespace Lattice
