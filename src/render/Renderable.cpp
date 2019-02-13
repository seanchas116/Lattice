#include "Renderable.hpp"

namespace Lattice {
namespace Render {

void Renderable::draw(const SP<Operations> &operations, const SP<Camera> &camera) {
    Q_UNUSED(operations); Q_UNUSED(camera);
}

void Renderable::drawPickables(const SP<Operations> &operations, const SP<Camera> &camera) {
    Q_UNUSED(operations); Q_UNUSED(camera);
}

Opt<HitResult> Renderable::hitTest(glm::dvec2 pos, const SP<Camera> &camera) const {
    Q_UNUSED(pos); Q_UNUSED(camera);
    return {};
}

} // namespace Renderer
} // namespace Lattice
