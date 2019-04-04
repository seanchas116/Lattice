#include "CenterHandle.hpp"
#include "../../gl/VAO.hpp"
#include "../../gl/Vertex.hpp"
#include "../../gl/VertexBuffer.hpp"
#include "../../support/Debug.hpp"
#include "../../drawable/PointsDrawable.hpp"

using namespace glm;

namespace Lattice {
namespace Editor {
namespace Manipulator {

CenterHandle::CenterHandle() {
}

void CenterHandle::draw(const SP<Render::Operations> &operations, const SP<Camera> &camera) {
    Q_UNUSED(operations); Q_UNUSED(camera);
}

void CenterHandle::drawPickables(const SP<Render::Operations> &operations, const SP<Camera> &camera) {
    auto [viewportPos, isInViewport] = camera->mapWorldToViewport(_targetPosition);
    if (!isInViewport) {
        return;
    }
    dvec3 frontPos(viewportPos.xy, 0);

    Drawable::PointsDrawable drawable;
    drawable.setPoints({{frontPos, toIDColor()}});
    drawable.setWidth(32);
    drawable.draw2D(operations->singletonBag, dmat4(1), camera->viewportSize());
}

void CenterHandle::mousePressEvent(const Render::MouseEvent &event) {
    if (event.originalEvent->button() != Qt::LeftButton) {
        return;
    }
    auto [viewportPos, isInViewport] = event.camera->mapWorldToViewport(_targetPosition);
    if (!isInViewport) {
        return;
    }
    qDebug() << "center press";
    _depth = viewportPos.z;
    _dragged = true;
    auto pos = event.camera->mapViewportToWorld(dvec3(dvec2(event.viewportPos), _depth));
    emit onDragBegin(pos);
}

void CenterHandle::mouseMoveEvent(const Render::MouseEvent &event) {
    if (!_dragged) {
        return;
    }
    auto pos = event.camera->mapViewportToWorld(dvec3(dvec2(event.viewportPos), _depth));
    emit onDragMove(pos);
}

void CenterHandle::mouseReleaseEvent(const Render::MouseEvent &event) {
    Q_UNUSED(event);
    if (!_dragged) {
        return;
    }
    _dragged = false;
    emit onDragEnd();
}

void CenterHandle::contextMenuEvent(const Render::ContextMenuEvent &event) {
    emit onContextMenu(event);
}

} // namespace Manipulator
} // namespace Editor
} // namespace Lattice
