#include "Renderable.hpp"

namespace Lattice {
namespace Render {

Renderable::~Renderable() {
}

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

glm::vec4 Renderable::toIDColor() const {
    union {
        const Renderable* ptr;
        glm::u16vec4 color;
    } idColor;
    idColor.ptr = this;

    return glm::vec4(idColor.color) / float(0xFFFF);
}

Opt<SP<Renderable> > Renderable::fromIDColor(glm::vec4 color) {
    union {
        Renderable* ptr;
        glm::u16vec4 color;
    } idColor;
    idColor.color = glm::u16vec4(glm::round(color * float(0xFFFF)));
    if (!idColor.ptr) {
        return {};
    }
    return idColor.ptr->sharedFromThis();
}

} // namespace Render
} // namespace Lattice
