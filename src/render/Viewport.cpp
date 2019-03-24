#include "Viewport.hpp"
#include "RenderableObject.hpp"
#include "Util.hpp"
#include "PickableMap.hpp"
#include "../support/Debug.hpp"
#include <QMouseEvent>
#include <QOpenGLDebugLogger>
#include <QResizeEvent>

namespace Lattice {
namespace Render {

Viewport::Viewport(QWidget *parent) : QWidget(parent), _camera(makeShared<Camera>()) {
    connect(_camera.get(), &Camera::changed, this, &Viewport::updateRequested);
    setMouseTracking(true);
}

void Viewport::setRenderable(const Opt<SP<Renderable> > &renderable) {
    auto renderableObject = dynamicPointerCast<RenderableObject>(renderable);
    if (renderableObject) {
        connect(renderableObject->get(), &RenderableObject::updated, this, &Viewport::updateRequested);
    }
    _renderable = renderable;
}

const SP<PickableMap> &Viewport::pickableMap() {
    if (!_pickableMap) {
        _pickableMap = makeShared<PickableMap>();
    }
    return *_pickableMap;
}

void Viewport::mousePressEvent(QMouseEvent *event) {
    auto pos = mapQtToGL(this, event->pos());

    auto maybeHitResult = hitTest(pos, _camera);
    if (!maybeHitResult) { return; }

    auto [renderable, hitDepth] = *maybeHitResult;

    MouseEvent renderMouseEvent(event, pos, _camera, hitDepth);
    renderable->mousePress(renderMouseEvent);
    _draggedRenderable = renderable;
    _hitDepth = hitDepth;
}

void Viewport::mouseMoveEvent(QMouseEvent *event) {
    auto pos = mapQtToGL(this, event->pos());

    if (_draggedRenderable) {
        // drag
        auto renderable = *_draggedRenderable;
        MouseEvent renderMouseEvent(event, pos, _camera, _hitDepth);
        renderable->mouseMove(renderMouseEvent);
        return;
    } else {
        // hover
        auto maybeHitResult = hitTest(pos, _camera);
        if (!maybeHitResult) {
            if (_hoveredRenderable) {
                (*_hoveredRenderable)->hoverLeave();
            }
            return;
        }
        auto [renderable, hitResult] = *maybeHitResult;
        MouseEvent renderMouseEvent(event, pos, _camera, hitResult);
        if (_hoveredRenderable == renderable) {
            renderable->mouseMove(renderMouseEvent);
        } else {
            if (_hoveredRenderable) {
                (*_hoveredRenderable)->hoverLeave();
            }
            renderable->hoverEnter(renderMouseEvent);
            renderable->mouseMove(renderMouseEvent);
            _hoveredRenderable = renderable;
        }
    }
}

void Viewport::mouseDoubleClickEvent(QMouseEvent *event) {
    auto pos = mapQtToGL(this, event->pos());

    auto maybeHitResult = hitTest(pos, _camera);
    if (!maybeHitResult) { return; }

    auto [renderable, hitResult] = *maybeHitResult;

    MouseEvent renderMouseEvent(event, pos, _camera, hitResult);
    renderable->mouseDoubleClick(renderMouseEvent);
}

void Viewport::contextMenuEvent(QContextMenuEvent *event) {
    auto pos = mapQtToGL(this, event->pos());

    auto maybeHitResult = hitTest(pos, _camera);
    if (!maybeHitResult) { return; }

    auto [renderable, hitResult] = *maybeHitResult;

    ContextMenuEvent renderContextMenuEvent(event, pos, _camera, hitResult);
    renderable->contextMenu(renderContextMenuEvent);
}

void Viewport::moveEvent(QMoveEvent *event) {
    super::moveEvent(event);
    emit updateRequested();
}

void Viewport::resizeEvent(QResizeEvent *event) {
    super::resizeEvent(event);
    _camera->setViewportSize(mapQtToGL(this, QPoint(event->size().width(), 0)));
    emit updateRequested();
}

void Viewport::mouseReleaseEvent(QMouseEvent *event) {
    LATTICE_OPTIONAL_GUARD(renderable, _draggedRenderable, return;)
    MouseEvent renderMouseEvent(event, mapQtToGL(this, event->pos()), _camera, _hitDepth);
    renderable->mouseRelease(renderMouseEvent);
    _draggedRenderable = {};
}

Opt<std::pair<SP<Renderable>, double> > Viewport::hitTest(glm::dvec2 pos, const SP<Camera> &camera) {
    Q_UNUSED(camera);

    if (!_pickableMap) {
        return {};
    }
    return _pickableMap->get()->pick(pos);
}

} // namespace Render
} // namespace Lattice
