#include "CylinderBuilder.hpp"
#include "CircleBuilder.hpp"
#include "../algorithm/Extrude.hpp"
#include <QtGlobal>

using namespace glm;

namespace Lattice {
namespace Mesh {

Mesh CylinderBuilder::build() const {
    CircleBuilder circleBuilder;
    circleBuilder.center = center;
    circleBuilder.radius = radius;
    circleBuilder.segmentCount = segmentCount;
    circleBuilder.normalAxis = axis;

    Mesh mesh = circleBuilder.build();
    auto extrudedVertices = extrude(mesh, mesh.vertices() | ranges::to_vector, true);

    vec3 extrudeOffset(0);
    extrudeOffset[axis] = height;

    for (auto&& v : extrudedVertices) {
        mesh.setPosition(v, mesh.position(v) + extrudeOffset);
    }

    return mesh;
}

} // namespace Mesh
} // namespace Lattice
