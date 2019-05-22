#include "Viewport.hpp"
#include "Renderable.hpp"
#include "Util.hpp"
#include "HitAreaMap.hpp"
#include "../support/Debug.hpp"
#include <QMouseEvent>
#include <QOpenGLDebugLogger>
#include <QResizeEvent>

namespace Lattice {
namespace Viewport {

Viewport::Viewport(QWidget *parent) : QWidget(parent), _camera(makeShared<Camera>()) {
    connect(_camera.get(), &Camera::changed, this, &Viewport::updateRequested);
    setMouseTracking(true);
}

void Viewport::setRenderable(const Opt<SP<Renderable> > &renderable) {
    if (renderable) {
        connect(renderable->get(), &Renderable::updated, this, &Viewport::updateRequested);
    }
    _renderable = renderable;
}

const SP<HitAreaMap> &Viewport::hitAreaMap() {
    if (!_hitAreaMap) {
        _hitAreaMap = makeShared<HitAreaMap>();
    }
    return *_hitAreaMap;
}

void Viewport::mousePressEvent(QMouseEvent *event) {
    auto pos = mapQtToGL(this, event->pos());

    auto maybeHitResult = hitTest(pos, _camera);
    if (!maybeHitResult) { return; }

    auto hitResult = *maybeHitResult;

    MouseEvent renderMouseEvent {glm::dvec3(pos, hitResult.depth), _camera, event, nullptr};
    hitResult.renderable->mousePressEvent(renderMouseEvent);
    _draggedHitResult = hitResult;
}

void Viewport::mouseMoveEvent(QMouseEvent *event) {
    auto pos = mapQtToGL(this, event->pos());

    if (_draggedHitResult) {
        // drag
        auto hitResult = *_draggedHitResult;
        MouseEvent renderMouseEvent {glm::dvec3(pos, hitResult.depth), _camera, event, nullptr};
        hitResult.renderable->mouseMoveEvent(renderMouseEvent);
        return;
    } else {
        // hover
        auto maybeHitResult = hitTest(pos, _camera);
        if (!maybeHitResult) {
            if (_hoveredHitResult) {
                (*_hoveredHitResult).renderable->hoverLeaveEvent();
            }
            return;
        }
        auto hitResult = *maybeHitResult;
        MouseEvent renderMouseEvent {glm::dvec3(pos, hitResult.depth), _camera, event, nullptr};
        if (_hoveredHitResult && _hoveredHitResult->renderable == hitResult.renderable) {
            hitResult.renderable->mouseMoveEvent(renderMouseEvent);
        } else {
            if (_hoveredHitResult) {
                _hoveredHitResult->renderable->hoverLeaveEvent();
            }
            hitResult.renderable->hoverEnterEvent(renderMouseEvent);
            hitResult.renderable->mouseMoveEvent(renderMouseEvent);
            _hoveredHitResult = hitResult;
        }
    }
}

void Viewport::mouseDoubleClickEvent(QMouseEvent *event) {
    auto pos = mapQtToGL(this, event->pos());

    auto maybeHitResult = hitTest(pos, _camera);
    if (!maybeHitResult) { return; }

    auto [renderable, hitDepth] = *maybeHitResult;

    MouseEvent renderMouseEvent {glm::dvec3(pos, hitDepth), _camera, event, nullptr};
    renderable->mouseDoubleClickEvent(renderMouseEvent);
}

void Viewport::contextMenuEvent(QContextMenuEvent *event) {
    auto pos = mapQtToGL(this, event->pos());

    auto maybeHitResult = hitTest(pos, _camera);
    if (!maybeHitResult) { return; }

    auto [renderable, hitDepth] = *maybeHitResult;

    MouseEvent renderContextMenuEvent {glm::dvec3(pos, hitDepth), _camera, nullptr, event};
    renderable->contextMenuEvent(renderContextMenuEvent);
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
    if (!_draggedHitResult) {
        return;
    }
    auto hitResult = *_draggedHitResult;

    MouseEvent renderMouseEvent {glm::dvec3(mapQtToGL(this, event->pos()), hitResult.depth), _camera, event, nullptr};
    hitResult.renderable->mouseReleaseEvent(renderMouseEvent);
    _draggedHitResult = {};
}

Opt<HitResult> Viewport::hitTest(glm::dvec2 pos, const SP<Camera> &camera) {
    Q_UNUSED(camera);

    if (!_hitAreaMap) {
        return {};
    }
    return _hitAreaMap->get()->pick(pos);
}

} // namespace Render
} // namespace Lattice
