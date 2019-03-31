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

void CenterHandle::draw(const SP<Render::Operations> &operations, const SP<Camera> &camera) {
    Q_UNUSED(operations); Q_UNUSED(camera);

}

void CenterHandle::drawPickables(const SP<Render::Operations> &operations, const SP<Camera> &camera) {
    auto [viewportPos, isInViewport] = camera->mapWorldToViewport(_targetPosition);
    if (!isInViewport) {
        return;
    }
    dvec3 frontPos(viewportPos.xy, 0);

    GL::Vertex vertex { frontPos };
    auto vbo = makeShared<GL::VertexBuffer<GL::Vertex>>();
    vbo->setVertices({vertex});
    auto vao = makeShared<GL::VAO>(vbo, GL::Primitive::Point);
    operations->drawCircle.draw2D(vao, dmat4(1), camera->viewportSize(), 32, toIDColor());
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
    emit onBegin(pos);
}

void CenterHandle::mouseMoveEvent(const Render::MouseEvent &event) {
    if (!_dragged) {
        return;
    }
    auto pos = event.camera->mapViewportToWorld(dvec3(dvec2(event.viewportPos), _depth));
    emit onChange(pos);
}

void CenterHandle::mouseReleaseEvent(const Render::MouseEvent &event) {
    Q_UNUSED(event);
    if (!_dragged) {
        return;
    }
    _dragged = false;
    emit onEnd();
}

void CenterHandle::contextMenuEvent(const Render::ContextMenuEvent &event) {
    emit onContextMenu(event);
}

} // namespace Manipulator
} // namespace Editor
} // namespace Lattice
