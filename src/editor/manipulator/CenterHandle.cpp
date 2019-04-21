#include "CenterHandle.hpp"
#include "../../gl/VAO.hpp"
#include "../../gl/Vertex.hpp"
#include "../../gl/VertexBuffer.hpp"
#include "../../support/Debug.hpp"

using namespace glm;

namespace Lattice {
namespace Editor {
namespace Manipulator {

CenterHandle::CenterHandle() {
}

void CenterHandle::draw(const SP<Draw::Operations> &operations, const SP<Camera> &camera) {
    Q_UNUSED(operations); Q_UNUSED(camera);
}

void CenterHandle::drawPickables(const SP<Draw::Operations> &operations, const SP<Camera> &camera) {
    auto [viewportPos, isInViewport] = camera->mapWorldToViewport(_targetPosition);
    if (!isInViewport) {
        return;
    }
    dvec3 frontPos(viewportPos.xy(), 0);

    GL::Vertex vertex { frontPos };
    auto vbo = makeShared<GL::VertexBuffer<GL::Vertex>>();
    vbo->setVertices({vertex});
    auto vao = makeShared<GL::VAO>(vbo, GL::Primitive::Point);
    operations->drawCircle.draw2D(vao, dmat4(1), camera->viewportSize(), 32, toIDColor());
}

void CenterHandle::mousePressEvent(const Viewport::MouseEvent &event) {
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

void CenterHandle::mouseMoveEvent(const Viewport::MouseEvent &event) {
    if (!_dragged) {
        return;
    }
    auto pos = event.camera->mapViewportToWorld(dvec3(dvec2(event.viewportPos), _depth));
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

void CenterHandle::contextMenuEvent(const Viewport::ContextMenuEvent &event) {
    emit onContextMenu(event);
}

} // namespace Manipulator
} // namespace Editor
} // namespace Lattice
