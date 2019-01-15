#include "MeshSelection.hpp"

using namespace glm;

namespace Lattice {
namespace Document {

glm::vec3 MeshSelection::medianPosition() const {
    if (vertices.empty()) {
        return vec3(0);
    }

    vec3 minPos(INFINITY);
    vec3 maxPos(-INFINITY);

    for (auto& v : vertices) {
        auto p = v->position();
        minPos = min(minPos, p);
        maxPos = max(maxPos, p);
    }

    return (minPos + maxPos) / 2.f;
}

} // namespace Document
} // namespace Lattice
