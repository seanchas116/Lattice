#include "BorderSelectTool.hpp"
#include "../../document/Document.hpp"
#include "../../document/History.hpp"
#include "../../support/Debug.hpp"

using namespace glm;

namespace Lattice {
namespace Editor {
namespace MeshEditor {

void BorderSelectTool::mousePressTool(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    Q_UNUSED(target);

    if (event.originalEvent->button() != Qt::LeftButton) {
        return;
    }
    _dragged = true;
    _initViewportPos = event.viewportPos;
    qDebug() << "dragging";
}

void BorderSelectTool::mouseMoveTool(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);

    if (!_dragged) {
        return;
    }
    // TODO
}

void BorderSelectTool::mouseReleaseTool(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);

    _dragged = false;

    // TODO
}

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
