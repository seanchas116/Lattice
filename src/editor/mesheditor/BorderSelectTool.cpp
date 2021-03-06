#include "BorderSelectTool.hpp"
#include "../../document/Document.hpp"
#include "../../document/History.hpp"
#include "../../support/Debug.hpp"
#include <QMouseEvent>
#include <QPainter>
#include <meshlib/Mesh.hpp>

using namespace glm;

namespace Lattice {
namespace Editor {
namespace MeshEditor {

void BorderSelectTool::mousePressTool(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    Q_UNUSED(target);

    if (event.originalMouseEvent->button() != Qt::LeftButton) {
        return;
    }
    _draggedViewport = event.viewport;
    _initViewportPos = _currentViewportPos = event.viewportPos;

    _vertices.clear();
    _vertices.reserve(mesh()->allVertexCount());
    for (auto vertex : mesh()->vertices()) {
        // TODO: optimize
        auto [screenPos, isInScreen] = event.camera.mapModelToViewport(object()->location().matrixToWorld(), mesh()->position(vertex));
        if (!isInScreen) {
            continue;
        }
        _vertices.push_back({vertex, screenPos.xy});
    }

    mesh()->deselectAll();

    emit updated();
    meshEditState()->notifyMeshChanged();
}

void BorderSelectTool::mouseMoveTool(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    Q_UNUSED(target);
    Q_UNUSED(event);

    if (!_draggedViewport) {
        return;
    }
    _currentViewportPos = event.viewportPos;

    auto minPos = min(_initViewportPos, _currentViewportPos);
    auto maxPos = max(_initViewportPos, _currentViewportPos);

    mesh()->deselectAll();

    for (auto &[vertex, screenPos] : _vertices) {
        if (minPos.x <= screenPos.x && minPos.y <= screenPos.y && screenPos.x <= maxPos.x && screenPos.y <= maxPos.y) {
            mesh()->setSelected(vertex, true);
        }
    }

    emit updated();
    meshEditState()->notifyMeshChanged();
}

void BorderSelectTool::mouseReleaseTool(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    Q_UNUSED(target);
    Q_UNUSED(event);

    _draggedViewport = nullptr;
    _vertices.clear();
    emit updated();
}

void BorderSelectTool::draw2D(const Viewport::Draw2DEvent &event) {
    if (event.viewport != _draggedViewport) {
        return;
    }

    auto minPos = min(_initViewportPos, _currentViewportPos);
    auto maxPos = max(_initViewportPos, _currentViewportPos);
    auto size = maxPos - minPos;

    event.painter->setBrush(QColor(255, 255, 255, 50));

    event.painter->drawRect(minPos.x, minPos.y, size.x, size.y);
}

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
