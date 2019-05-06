#include "AddPlane.hpp"
#include <QtGlobal>

using namespace glm;

namespace Lattice {
namespace Mesh {

Mesh AddPlane::perform(const Mesh &original) {
    auto mesh = original;

    std::vector<UVPointHandle> uvPoints;
    std::array<vec2, 4> uvs = {vec2(0, 0), vec2(1, 0), vec2(1, 1), vec2(0, 1)};

    for (vec2 uv : uvs) {
        vec3 pos = center;
        pos[(normalAxis + 1) % 3] += size.x * (uv.x - 0.5f);
        pos[(normalAxis + 2) % 3] += size.y * (uv.y - 0.5f);
        auto v = mesh.addVertex(pos);
        auto uvPoint = mesh.addUVPoint(v, uv);
        uvPoints.push_back(uvPoint);
    }
    mesh.addFace(uvPoints, material);

    return mesh;
}

} // namespace Mesh
} // namespace Lattice
