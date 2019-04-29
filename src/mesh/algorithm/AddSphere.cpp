#include "AddSphere.hpp"
#include <QtGlobal>

using namespace glm;

namespace Lattice {
namespace Mesh {

void AddSphere::redo(Mesh &mesh) {
    std::vector<std::vector<UVPointHandle>> uvPointMatrix;

    for (int ring = 0; ring < ringCount - 1; ++ring) {
        std::vector<UVPointHandle> uvPoints;

        float longitude = float(M_PI) * (ring + 1 - ringCount * 0.5f) / ringCount;

        for (int i = 0; i < segmentCount; ++i) {
            float latitude = float(M_PI) * 2.f * (float(i) / segmentCount);
            vec3 offset;
            offset[axis] = sin(longitude);
            offset[(axis + 1) % 3] = cos(latitude) * cos(longitude);
            offset[(axis + 2) % 3] = sin(latitude) * cos(longitude);
            vec3 pos = center + offset * radius;
            uvPoints.push_back(mesh.addUVPoint(mesh.addVertex(pos), dvec2(0)));
        }

        uvPointMatrix.push_back(std::move(uvPoints));
    }

    auto bottom = mesh.addUVPoint(mesh.addVertex(center + vec3(0, -radius, 0)), vec2(0));
    auto top = mesh.addUVPoint(mesh.addVertex(center + vec3(0, radius, 0)), vec2(0));

    for (int i = 0; i < segmentCount; ++i) {
        int next = (i + 1) % segmentCount;
        mesh.addFace({bottom, uvPointMatrix[0][next], uvPointMatrix[0][i]}, material);

        for (int ring = 0; ring < ringCount - 2; ++ring) {
            mesh.addFace({uvPointMatrix[ring][i], uvPointMatrix[ring][next], uvPointMatrix[ring+1][next], uvPointMatrix[ring+1][i]}, material);
        }

        mesh.addFace({uvPointMatrix[ringCount - 2][i], uvPointMatrix[ringCount - 2][next], top}, material);
    }

    vertices.clear();
    vertices.push_back(mesh.vertex(bottom));
    for (auto& uvPoints : uvPointMatrix) {
        for (auto uvPoint : uvPoints) {
            vertices.push_back(mesh.vertex(uvPoint));
        }
    }
    vertices.push_back(mesh.vertex(top));
}

void AddSphere::undo(Mesh &mesh) {
    for (auto v : vertices) {
        mesh.removeVertex(v);
    }
}

} // namespace Mesh
} // namespace Lattice
