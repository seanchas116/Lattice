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

glm::vec4 Pickable::idColor() const {
    union {
        uint64_t id;
        glm::u16vec4 color;
    } idColor;
    idColor.id = reinterpret_cast<uint64_t>(this);

    return glm::vec4(idColor.color) / float(0xFFFF);
}

} // namespace Render
} // namespace Lattice
