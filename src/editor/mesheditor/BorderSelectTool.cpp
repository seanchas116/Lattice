#include "BorderSelectTool.hpp"
#include "../../document/Document.hpp"
#include "../../document/History.hpp"
#include "../../support/Debug.hpp"
#include <QPainter>

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
    _initViewportPos = _currentViewportPos = event.viewportPos;

    _vertices.clear();
    _vertices.reserve(object()->mesh()->vertices().size());
    for (auto& vertex : object()->mesh()->vertices()) {
        // TODO: optimize
        auto [screenPos, isInScreen] = event.camera->mapModelToViewport(object()->location().matrixToWorld(), vertex->position());
        if (!isInScreen) {
            continue;
        }
        _vertices.push_back({vertex, screenPos.xy});
    }

    emit overlayUpdated();
}

void BorderSelectTool::mouseMoveEvent(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);

    if (!_dragged) {
        return;
    }
    _currentViewportPos = event.viewportPos;

    auto minPos = min(_initViewportPos, _currentViewportPos);
    auto maxPos = max(_initViewportPos, _currentViewportPos);

    Mesh::MeshFragment selection;

    for (auto& [vertex, screenPos] : _vertices) {
        if (minPos.x <= screenPos.x && minPos.y <= screenPos.y && screenPos.x <= maxPos.x && screenPos.y <= maxPos.y) {
            selection.vertices.insert(vertex);
        }
    }

    appState()->document()->setMeshSelection(selection);
    emit overlayUpdated();
}

void BorderSelectTool::mouseReleaseEvent(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);

    _dragged = false;
    _vertices.clear();
    emit overlayUpdated();
}

void BorderSelectTool::drawOverlay(QPainter *painter, const QSize &viewportSize) {
    Q_UNUSED(viewportSize);
    if (!_dragged) {
        return;
    }

    auto minPos = min(_initViewportPos, _currentViewportPos);
    auto maxPos = max(_initViewportPos, _currentViewportPos);
    auto size = maxPos - minPos;

    painter->setBrush(QColor(255, 255, 255, 50));

    painter->drawRect(minPos.x, minPos.y, size.x, size.y);
}

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
