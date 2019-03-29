#include "CenterHandle.hpp"
#include "../../gl/VAO.hpp"
#include "../../gl/Vertex.hpp"
#include "../../gl/VertexBuffer.hpp"

using namespace glm;

namespace Lattice {
namespace Editor {
namespace Manipulator {

CenterHandle::CenterHandle() {
}

void CenterHandle::draw(const SP<Render::Operations> &operations, const SP<Camera> &camera) {
    Q_UNUSED(operations); Q_UNUSED(camera);
    auto [viewportPos, isInViewport] = camera->mapWorldToViewport(_targetPosition);
    if (!isInViewport) {
        return;
    }
    dvec3 frontPos(viewportPos.xy, 0);

    GL::Vertex vertex { frontPos };
    auto vbo = makeShared<GL::VertexBuffer<GL::Vertex>>();
    vbo->setVertices({vertex});
    auto vao = makeShared<GL::VAO>(vbo, GL::Primitive::Point);
    operations->drawCircle.draw2D(vao, dmat4(1), camera->viewportSize(), 10, glm::vec4(1, 0, 0, 1));
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
