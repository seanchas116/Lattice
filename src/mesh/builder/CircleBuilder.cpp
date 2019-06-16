#include "CircleBuilder.hpp"
#include <QtGlobal>

using namespace glm;

namespace Lattice {
namespace Mesh {

Mesh CircleBuilder::build() const {
    Mesh mesh;

    std::vector<UVPointHandle> uvPoints;

    float angleStep = float(M_PI) * 2.f / segmentCount;
    for (int i = 0 ; i < segmentCount; ++i) {
        float angle = angleStep * i;
        vec3 offset(0);
        offset[(normalAxis + 1) % 3] = cos(angle);
        offset[(normalAxis + 2) % 3] = sin(angle);
        vec3 pos = center + offset * radius;
        auto v = mesh.addVertex(pos);
        auto uv = mesh.addUVPoint(v, vec2(0));
        uvPoints.push_back(uv);
    }

    mesh.addFace(uvPoints, material);

    return mesh;
}

} // namespace Mesh
} // namespace Lattice
