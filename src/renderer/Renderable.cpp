#include "Renderable.hpp"

namespace Lattice {
namespace Renderer {

Renderable::Renderable() {
}

Renderable::~Renderable() {
}

void Renderable::draw(const SP<Operations> &operations, const Camera &camera) {
    Q_UNUSED(operations); Q_UNUSED(camera);
}

std::pair<bool, double> Renderable::hitTest(glm::dvec2 pos, const Camera &camera) const {
    Q_UNUSED(pos); Q_UNUSED(camera);
    return {false, 0};
}

void Renderable::mousePress(QMouseEvent *event, glm::dvec2 pos, const Camera &camera) {
    Q_UNUSED(event); Q_UNUSED(pos); Q_UNUSED(camera);
}

void Renderable::mouseMove(QMouseEvent *event, glm::dvec2 pos, const Camera &camera) {
    Q_UNUSED(event); Q_UNUSED(pos); Q_UNUSED(camera);
}

void Renderable::mouseRelease(QMouseEvent *event, glm::dvec2 pos, const Camera &camera) {
    Q_UNUSED(event); Q_UNUSED(pos); Q_UNUSED(camera);
}

} // namespace Renderer
} // namespace Lattice
