#include "CenterPickable.hpp"

namespace Lattice {
namespace Editor {
namespace Manipulator {

CenterPickable::CenterPickable() {
}

void CenterPickable::draw(const SP<Render::Operations> &operations, const SP<Camera> &camera) {
    Q_UNUSED(operations); Q_UNUSED(camera);
}

void CenterPickable::drawPickables(const SP<Render::Operations> &operations, const SP<Camera> &camera) {
    Q_UNUSED(operations); Q_UNUSED(camera);
}

void CenterPickable::mousePressEvent(const Render::MouseEvent &event) {
    Q_UNUSED(event);
}

void CenterPickable::mouseMoveEvent(const Render::MouseEvent &event) {
    Q_UNUSED(event);
}

void CenterPickable::mouseReleaseEvent(const Render::MouseEvent &event) {
    Q_UNUSED(event);
}

void CenterPickable::contextMenuEvent(const Render::ContextMenuEvent &event) {
    emit onContextMenu(event);
}

} // namespace Manipulator
} // namespace Editor
} // namespace Lattice
