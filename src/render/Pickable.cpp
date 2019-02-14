#include "Pickable.hpp"

namespace Lattice {
namespace Render {

Pickable::~Pickable() {
}

void Pickable::mousePress(const MouseEvent &event) {
    Q_UNUSED(event);
}

void Pickable::mouseMove(const MouseEvent &event) {
    Q_UNUSED(event);
}

void Pickable::mouseRelease(const MouseEvent &event) {
    Q_UNUSED(event);
}

void Pickable::mouseDoubleClick(const MouseEvent &event) {
    Q_UNUSED(event);
}

void Pickable::hoverEnter(const MouseEvent &event) {
    Q_UNUSED(event);
}

void Pickable::hoverMove(const MouseEvent &event) {
    Q_UNUSED(event);
}

void Pickable::hoverLeave() {
}

glm::vec4 Pickable::toIDColor() const {
    union {
        const Pickable* ptr;
        glm::u16vec4 color;
    } idColor;
    idColor.ptr = this;

    return glm::vec4(idColor.color) / float(0xFFFF);
}

Opt<SP<Pickable> > Pickable::fromIDColor(glm::vec4 color) {
    union {
        Pickable* ptr;
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
