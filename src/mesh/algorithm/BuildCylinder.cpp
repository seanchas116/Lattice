#include "BuildCylinder.hpp"
#include <QtGlobal>

using namespace glm;

namespace Lattice {
namespace Mesh {

Mesh BuildCylinder::perform() const {
    Mesh mesh;

    float angleStep = M_PI * 2 / segmentCount;

    std::vector<UVPointHandle> bottomUVPoints;
    bottomUVPoints.reserve(segmentCount);

    for (int i = 0 ; i < segmentCount; ++i) {
        float angle = angleStep * i;
        vec3 offset(0);
        offset[(axis + 1) % 3] = cos(angle) * radius;
        offset[(axis + 2) % 3] = sin(angle) * radius;
        vec3 pos = center + offset;
        auto v = mesh.addUVPoint(mesh.addVertex(pos), vec2(0));
        bottomUVPoints.push_back(v);
    }

    std::vector<UVPointHandle> reverseBottomUVPoints(bottomUVPoints.rbegin(), bottomUVPoints.rend());
    mesh.addFace(reverseBottomUVPoints, material);

    std::vector<UVPointHandle> topUVPoints;
    topUVPoints.reserve(segmentCount);

    for (int i = 0 ; i < segmentCount; ++i) {
        float angle = angleStep * i;
        vec3 offset(0);
        offset[(axis + 1) % 3] = cos(angle) * radius;
        offset[(axis + 2) % 3] = sin(angle) * radius;
        offset[axis] = height;
        vec3 pos = center + offset;
        auto v = mesh.addUVPoint(mesh.addVertex(pos), vec2(0));
        topUVPoints.push_back(v);
    }

    mesh.addFace(topUVPoints, material);

    for (int i = 0; i < segmentCount; ++i) {
        auto v0 = bottomUVPoints[i];
        auto v1 = bottomUVPoints[(i + 1) % segmentCount];
        auto v2 = topUVPoints[(i + 1) % segmentCount];
        auto v3 = topUVPoints[i];
        mesh.addFace({v0, v1, v2, v3}, material);
    }

    return mesh;
}

} // namespace Mesh
} // namespace Lattice
