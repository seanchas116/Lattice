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

void Renderable::mousePress(const MouseEvent &event) {
    Q_UNUSED(event);
}

void Renderable::mouseMove(const MouseEvent &event) {
    Q_UNUSED(event);
}

void Renderable::mouseRelease(const MouseEvent &event) {
    Q_UNUSED(event);
}

} // namespace Renderer
} // namespace Lattice
