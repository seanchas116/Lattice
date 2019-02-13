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

Pickable *Pickable::fromIDColor(glm::i16vec4 color) {
    union {
        Pickable* ptr;
        glm::u16vec4 color;
    } idColor;
    idColor.color = color;
    return idColor.ptr;
}

} // namespace Render
} // namespace Lattice
