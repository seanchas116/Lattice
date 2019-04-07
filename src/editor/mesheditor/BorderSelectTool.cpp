#include "BorderSelectTool.hpp"
#include "../../document/Document.hpp"
#include "../../document/History.hpp"

using namespace glm;

namespace Lattice {
namespace Editor {
namespace MeshEditor {

void BorderSelectTool::mousePressEvent(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    Q_UNUSED(target);

    if (event.originalEvent->button() != Qt::LeftButton) {
        return;
    }
    _dragged = true;
    _initViewportPos = event.viewportPos;
}

void BorderSelectTool::mouseMoveEvent(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);

    if (!_dragged) {
        return;
    }
    // TODO
}

void BorderSelectTool::mouseReleaseEvent(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);

    _dragged = false;

    // TODO
}

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
