#include "CenterHandle.hpp"

namespace Lattice {
namespace Editor {
namespace Manipulator {

CenterHandle::CenterHandle() {
}

void CenterHandle::draw(const SP<Render::Operations> &operations, const SP<Camera> &camera) {
    Q_UNUSED(operations); Q_UNUSED(camera);
}

void CenterHandle::drawPickables(const SP<Render::Operations> &operations, const SP<Camera> &camera) {
    Q_UNUSED(operations); Q_UNUSED(camera);
}

void CenterHandle::mousePressEvent(const Render::MouseEvent &event) {
    Q_UNUSED(event);
}

void CenterHandle::mouseMoveEvent(const Render::MouseEvent &event) {
    Q_UNUSED(event);
}

void CenterHandle::mouseReleaseEvent(const Render::MouseEvent &event) {
    Q_UNUSED(event);
}

void CenterHandle::contextMenuEvent(const Render::ContextMenuEvent &event) {
    emit onContextMenu(event);
}

} // namespace Manipulator
} // namespace Editor
} // namespace Lattice
