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

} // namespace Render
} // namespace Lattice
