#include "AddCube.hpp"
#include <QtGlobal>

using namespace glm;

namespace Lattice {
namespace Mesh {

void AddCube::perform(Mesh &mesh) {
    //   2    3
    // 6    7
    //   0    1
    // 4    5

    auto v0 = mesh.addUVPoint(mesh.addVertex(vec3(minPos.x, minPos.y, minPos.z)), vec2(0));
    auto v1 = mesh.addUVPoint(mesh.addVertex(vec3(maxPos.x, minPos.y, minPos.z)), vec2(0));
    auto v2 = mesh.addUVPoint(mesh.addVertex(vec3(minPos.x, maxPos.y, minPos.z)), vec2(0));
    auto v3 = mesh.addUVPoint(mesh.addVertex(vec3(maxPos.x, maxPos.y, minPos.z)), vec2(0));
    auto v4 = mesh.addUVPoint(mesh.addVertex(vec3(minPos.x, minPos.y, maxPos.z)), vec2(0));
    auto v5 = mesh.addUVPoint(mesh.addVertex(vec3(maxPos.x, minPos.y, maxPos.z)), vec2(0));
    auto v6 = mesh.addUVPoint(mesh.addVertex(vec3(minPos.x, maxPos.y, maxPos.z)), vec2(0));
    auto v7 = mesh.addUVPoint(mesh.addVertex(vec3(maxPos.x, maxPos.y, maxPos.z)), vec2(0));

    mesh.addFace({v0, v4, v6, v2}, material);
    mesh.addFace({v1, v3, v7, v5}, material);
    mesh.addFace({v0, v1, v5, v4}, material);
    mesh.addFace({v2, v6, v7, v3}, material);
    mesh.addFace({v0, v2, v3, v1}, material);
    mesh.addFace({v4, v5, v7, v6}, material);
}

} // namespace Mesh
} // namespace Lattice
