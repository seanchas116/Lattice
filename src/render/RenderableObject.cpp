#include "RenderableObject.hpp"

namespace Lattice {
namespace Render {

void RenderableObject::draw(const SP<Operations> &operations, const SP<Camera> &camera) {
    Q_UNUSED(operations); Q_UNUSED(camera);
}

void RenderableObject::drawPickables(const SP<Operations> &operations, const SP<Camera> &camera) {
    Q_UNUSED(operations); Q_UNUSED(camera);
}

Opt<HitResult> RenderableObject::hitTest(glm::dvec2 pos, const SP<Camera> &camera) const {
    Q_UNUSED(pos); Q_UNUSED(camera);
    return {};
}

} // namespace Renderer
} // namespace Lattice
