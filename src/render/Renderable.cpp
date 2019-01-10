#include "Renderable.hpp"

namespace Lattice {
namespace Render {

Renderable::Renderable() {
}

Renderable::~Renderable() {
}

void Renderable::draw(const SP<Operations> &operations, const Camera &camera) {
    Q_UNUSED(operations); Q_UNUSED(camera);
}

std::optional<HitResult> Renderable::hitTest(glm::dvec2 pos, const Camera &camera) const {
    Q_UNUSED(pos); Q_UNUSED(camera);
    return {};
}

void Renderable::mousePress(QMouseEvent *event, glm::dvec2 pos, const Camera &camera, const HitResult &hitResult) {
    Q_UNUSED(event); Q_UNUSED(pos); Q_UNUSED(camera); Q_UNUSED(hitResult);
}

void Renderable::mouseMove(QMouseEvent *event, glm::dvec2 pos, const Camera &camera) {
    Q_UNUSED(event); Q_UNUSED(pos); Q_UNUSED(camera);
}

void Renderable::mouseRelease(QMouseEvent *event, glm::dvec2 pos, const Camera &camera) {
    Q_UNUSED(event); Q_UNUSED(pos); Q_UNUSED(camera);
}

} // namespace Renderer
} // namespace Lattice
