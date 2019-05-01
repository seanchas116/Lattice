#include "Renderable.hpp"

namespace Lattice {
namespace Viewport {

Renderable::~Renderable() {
}

void Renderable::draw(const SP<Draw::Operations> &operations, const SP<Camera> &camera) {
    Q_UNUSED(operations); Q_UNUSED(camera);
}

void Renderable::drawPickables(const SP<Draw::Operations> &operations, const SP<Camera> &camera, const PickableID &pickableID) {
    Q_UNUSED(operations); Q_UNUSED(camera); Q_UNUSED(pickableID);
}

void Renderable::draw2D(QPainter *painter, const QSize &viewportSize) {
    Q_UNUSED(painter); Q_UNUSED(viewportSize);
}

void Renderable::mousePressEvent(const MouseEvent &event) {
    Q_UNUSED(event);
}

void Renderable::mouseMoveEvent(const MouseEvent &event) {
    Q_UNUSED(event);
}

void Renderable::mouseReleaseEvent(const MouseEvent &event) {
    Q_UNUSED(event);
}

void Renderable::mouseDoubleClickEvent(const MouseEvent &event) {
    Q_UNUSED(event);
}

void Renderable::contextMenuEvent(const ContextMenuEvent &event) {
    Q_UNUSED(event);
}

void Renderable::hoverEnterEvent(const MouseEvent &event) {
    Q_UNUSED(event);
}

void Renderable::hoverLeaveEvent() {
}

} // namespace Render
} // namespace Lattice
