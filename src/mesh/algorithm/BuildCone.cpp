#include "BuildCone.hpp"
#include <QtGlobal>

using namespace glm;

namespace Lattice {
namespace Mesh {

Mesh BuildCone::perform() const {
    Mesh mesh;

    std::vector<UVPointHandle> uvPoints;

    float angleStep = float(M_PI) * 2 / segmentCount;
    for (int i = 0 ; i < segmentCount; ++i) {
        float angle = angleStep * i;
        vec3 offset(0);
        offset[(axis + 1) % 3] = cos(angle);
        offset[(axis + 2) % 3] = sin(angle);
        vec3 pos = center + offset * radius;
        auto uv = mesh.addUVPoint(mesh.addVertex(pos), dvec2(0));
        uvPoints.push_back(uv);
    }

    std::vector<UVPointHandle> reverseUVPoints(uvPoints.rbegin(), uvPoints.rend());
    mesh.addFace(reverseUVPoints, material);

    vec3 topPosition = center;
    topPosition[axis] += height;

    auto top = mesh.addUVPoint(mesh.addVertex(topPosition), vec2(0));

    for (int i = 0; i < segmentCount; ++i) {
        auto v0 = uvPoints[i];
        auto v1 = uvPoints[(i + 1) % segmentCount];
        mesh.addFace({v0, v1, top}, material);
    }

    return mesh;
}

} // namespace Mesh
} // namespace Lattice
