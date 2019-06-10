#include "CenterHandle.hpp"
#include "../../gl/VAO.hpp"
#include "../../draw/Vertex.hpp"
#include "../../gl/VertexBuffer.hpp"
#include "../../support/Debug.hpp"
#include "../../support/Camera.hpp"
#include <QMouseEvent>

using namespace glm;

namespace Lattice {
namespace Editor {
namespace Manipulator {

CenterHandle::CenterHandle() {
}

void CenterHandle::draw(const Viewport::DrawEvent &event) {
    Q_UNUSED(event);
}

void CenterHandle::drawHitArea(const Viewport::DrawEvent &event) {
    auto [viewportPos, isInViewport] = event.camera.mapWorldToViewport(_targetPosition);
    if (!isInViewport) {
        return;
    }
    dvec3 frontPos(viewportPos.xy, 0);

    Draw::Vertex vertex { frontPos };
    auto vbo = makeShared<GL::VertexBuffer<Draw::Vertex>>();
    vbo->setVertices({vertex});
    auto vao = makeShared<GL::VAO>(vbo, GL::Primitive::Point);
    event.operations->drawCircle.draw2D(vao, dmat4(1), event.camera.viewportSize(), 32, toIDColor());
}

void CenterHandle::mousePressEvent(const Viewport::MouseEvent &event) {
    if (event.originalMouseEvent->button() != Qt::LeftButton) {
        return;
    }
    auto [viewportPos, isInViewport] = event.camera.mapWorldToViewport(_targetPosition);
    if (!isInViewport) {
        return;
    }
    qDebug() << "center press";
    _depth = viewportPos.z;
    _dragged = true;
    auto pos = event.camera.mapViewportToWorld(dvec3(dvec2(event.viewportPos), _depth));
    emit onDragBegin(pos);
}

void CenterHandle::mouseMoveEvent(const Viewport::MouseEvent &event) {
    if (!_dragged) {
        return;
    }
    auto pos = event.camera.mapViewportToWorld(dvec3(dvec2(event.viewportPos), _depth));
    emit onDragMove(pos);
}

void CenterHandle::mouseReleaseEvent(const Viewport::MouseEvent &event) {
    Q_UNUSED(event);
    if (!_dragged) {
        return;
    }
    _dragged = false;
    emit onDragEnd();
}

void CenterHandle::contextMenuEvent(const Viewport::MouseEvent &event) {
    emit onContextMenu(event);
}

} // namespace Manipulator
} // namespace Editor
} // namespace Lattice
