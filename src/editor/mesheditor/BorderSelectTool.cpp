#include "BorderSelectTool.hpp"
#include "../../document/Document.hpp"
#include "../../document/History.hpp"
#include "../../support/Debug.hpp"
#include "../../gl/VAO.hpp"
#include "../../gl/Vertex.hpp"
#include "../../gl/VertexBuffer.hpp"

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
    _initViewportPos = _currentViewportPos = event.viewportPos;
    update();
}

void BorderSelectTool::mouseMoveTool(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);

    if (!_dragged) {
        return;
    }

    _currentViewportPos = event.viewportPos;
    update();
}

void BorderSelectTool::mouseReleaseTool(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);

    _dragged = false;
    update();
}

void BorderSelectTool::draw(const SP<Draw::Operations> &operations, const SP<Camera> &camera) {
    GL::Vertex vertex { dvec3(_currentViewportPos, 0) };
    auto vbo = makeShared<GL::VertexBuffer<GL::Vertex>>();
    vbo->setVertices({vertex});
    auto vao = makeShared<GL::VAO>(vbo, GL::Primitive::Point);
    operations->drawCircle.draw2D(vao, dmat4(1), camera->viewportSize(), 32, glm::vec4(0, 0, 1, 1));
}

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
