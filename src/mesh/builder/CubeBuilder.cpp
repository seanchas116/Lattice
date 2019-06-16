#include "CubeBuilder.hpp"
#include <QtGlobal>

using namespace glm;

namespace Lattice {
namespace Mesh {

Mesh CubeBuilder::build() const {
    Mesh mesh;

    //   2    3
    // 6    7
    //   0    1
    // 4    5

    auto v0 = mesh.addVertex(vec3(minPos.x, minPos.y, minPos.z));
    auto v1 = mesh.addVertex(vec3(maxPos.x, minPos.y, minPos.z));
    auto v2 = mesh.addVertex(vec3(minPos.x, maxPos.y, minPos.z));
    auto v3 = mesh.addVertex(vec3(maxPos.x, maxPos.y, minPos.z));
    auto v4 = mesh.addVertex(vec3(minPos.x, minPos.y, maxPos.z));
    auto v5 = mesh.addVertex(vec3(maxPos.x, minPos.y, maxPos.z));
    auto v6 = mesh.addVertex(vec3(minPos.x, maxPos.y, maxPos.z));
    auto v7 = mesh.addVertex(vec3(maxPos.x, maxPos.y, maxPos.z));

    auto uv0 = mesh.addUVPoint(v0, vec2(0));
    auto uv1 = mesh.addUVPoint(v1, vec2(0));
    auto uv2 = mesh.addUVPoint(v2, vec2(0));
    auto uv3 = mesh.addUVPoint(v3, vec2(0));
    auto uv4 = mesh.addUVPoint(v4, vec2(0));
    auto uv5 = mesh.addUVPoint(v5, vec2(0));
    auto uv6 = mesh.addUVPoint(v6, vec2(0));
    auto uv7 = mesh.addUVPoint(v7, vec2(0));

    mesh.addFace({uv0, uv4, uv6, uv2}, material);
    mesh.addFace({uv1, uv3, uv7, uv5}, material);
    mesh.addFace({uv0, uv1, uv5, uv4}, material);
    mesh.addFace({uv2, uv6, uv7, uv3}, material);
    mesh.addFace({uv0, uv2, uv3, uv1}, material);
    mesh.addFace({uv4, uv5, uv7, uv6}, material);

    return mesh;
}

} // namespace Mesh
} // namespace Lattice
