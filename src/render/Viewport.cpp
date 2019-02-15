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

    auto [renderable, hitResult] = *maybeHitResult;

    MouseEvent renderMouseEvent(event, pos, _camera, hitResult);
    renderable->mousePress(renderMouseEvent);
    _draggedRenderable = renderable;
    _hitResult = hitResult;
}

void Viewport::mouseMoveEvent(QMouseEvent *event) {
    auto pos = mapQtToGL(this, event->pos());

    if (_pickableMap) {
        auto pickable = _pickableMap->get()->pick(pos);
        qDebug() << pickable;
    }

    if (_draggedRenderable) {
        // drag
        auto renderable = *_draggedRenderable;
        MouseEvent renderMouseEvent(event, pos, _camera, _hitResult);
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
            renderable->hoverMove(renderMouseEvent);
        } else {
            if (_hoveredRenderable) {
                (*_hoveredRenderable)->hoverLeave();
            }
            renderable->hoverEnter(renderMouseEvent);
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

void Viewport::moveEvent(QMoveEvent *event) {
    super::moveEvent(event);
    emit updateRequested();
}

void Viewport::resizeEvent(QResizeEvent *event) {
    super::resizeEvent(event);
    _camera->setViewSize(mapQtToGL(this, QPoint(event->size().width(), 0)));
    emit updateRequested();
}

void Viewport::mouseReleaseEvent(QMouseEvent *event) {
    LATTICE_OPTIONAL_GUARD(renderable, _draggedRenderable, return;)
    MouseEvent renderMouseEvent(event, mapQtToGL(this, event->pos()), _camera, _hitResult);
    renderable->mouseRelease(renderMouseEvent);
    _draggedRenderable = {};
}

Opt<std::pair<SP<RenderableObject>, HitResult> > Viewport::hitTest(glm::dvec2 pos, const SP<Camera> &camera) {
    std::map<double, std::pair<SP<RenderableObject>, HitResult>> hitRenderables;
    for (auto it = _renderables.rbegin(); it != _renderables.rend(); ++it) {
        auto& renderable = *it;
        auto maybeHitResult = renderable->hitTest(pos, camera);
        if (maybeHitResult) {
            hitRenderables.insert({maybeHitResult->depth, {renderable, *maybeHitResult}});
        }
    }
    if (hitRenderables.empty()) {
        return std::nullopt;
    }
    return hitRenderables.begin()->second;
}

} // namespace Render
} // namespace Lattice
