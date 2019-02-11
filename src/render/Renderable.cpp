#include "Renderable.hpp"

namespace Lattice {
namespace Render {

Renderable::Renderable() {
}

void Renderable::draw(const SP<Operations> &operations, const SP<Camera> &camera) {
    Q_UNUSED(operations); Q_UNUSED(camera);
}

Opt<HitResult> Renderable::hitTest(glm::dvec2 pos, const SP<Camera> &camera) const {
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

void Renderable::mouseDoubleClick(const MouseEvent &event) {
    Q_UNUSED(event);
}

void Renderable::hoverEnter(const MouseEvent &event) {
    Q_UNUSED(event);
}

void Renderable::hoverMove(const MouseEvent &event) {
    Q_UNUSED(event);
}

void Renderable::hoverLeave() {
}

} // namespace Renderer
} // namespace Lattice
