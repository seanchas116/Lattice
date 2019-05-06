#include "Viewport.hpp"
#include "RenderableObject.hpp"
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

void Viewport::setRenderable(const Opt<SP<RenderableObject> > &renderable) {
    if (renderable) {
        connect(renderable->get(), &RenderableObject::updated, this, &Viewport::updateRequested);
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

    auto [renderable, hitDepth] = *maybeHitResult;

    MouseEvent renderMouseEvent(event, glm::dvec3(pos, hitDepth), _camera);
    renderable->mousePressEvent(renderMouseEvent);
    _draggedRenderable = renderable;
    _hitDepth = hitDepth;
}

void Viewport::mouseMoveEvent(QMouseEvent *event) {
    auto pos = mapQtToGL(this, event->pos());

    if (_draggedRenderable) {
        // drag
        auto renderable = *_draggedRenderable;
        MouseEvent renderMouseEvent(event, glm::dvec3(pos, _hitDepth), _camera);
        renderable->mouseMoveEvent(renderMouseEvent);
        return;
    } else {
        // hover
        auto maybeHitResult = hitTest(pos, _camera);
        if (!maybeHitResult) {
            if (_hoveredRenderable) {
                (*_hoveredRenderable)->hoverLeaveEvent();
            }
            return;
        }
        auto [renderable, hitDepth] = *maybeHitResult;
        MouseEvent renderMouseEvent(event, glm::dvec3(pos, hitDepth), _camera);
        if (_hoveredRenderable == renderable) {
            renderable->mouseMoveEvent(renderMouseEvent);
        } else {
            if (_hoveredRenderable) {
                (*_hoveredRenderable)->hoverLeaveEvent();
            }
            renderable->hoverEnterEvent(renderMouseEvent);
            renderable->mouseMoveEvent(renderMouseEvent);
            _hoveredRenderable = renderable;
        }
    }
}

void Viewport::mouseDoubleClickEvent(QMouseEvent *event) {
    auto pos = mapQtToGL(this, event->pos());

    auto maybeHitResult = hitTest(pos, _camera);
    if (!maybeHitResult) { return; }

    auto [renderable, hitDepth] = *maybeHitResult;

    MouseEvent renderMouseEvent(event, glm::dvec3(pos, hitDepth), _camera);
    renderable->mouseDoubleClickEvent(renderMouseEvent);
}

void Viewport::contextMenuEvent(QContextMenuEvent *event) {
    auto pos = mapQtToGL(this, event->pos());

    auto maybeHitResult = hitTest(pos, _camera);
    if (!maybeHitResult) { return; }

    auto [renderable, hitDepth] = *maybeHitResult;

    ContextMenuEvent renderContextMenuEvent(event, glm::dvec3(pos, hitDepth), _camera);
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
    LATTICE_OPTIONAL_GUARD(renderable, _draggedRenderable, return;)
    MouseEvent renderMouseEvent(event, glm::dvec3(mapQtToGL(this, event->pos()), _hitDepth), _camera);
    renderable->mouseReleaseEvent(renderMouseEvent);
    _draggedRenderable = {};
}

Opt<std::pair<SP<Renderable>, double> > Viewport::hitTest(glm::dvec2 pos, const SP<Camera> &camera) {
    Q_UNUSED(camera);

    if (!_hitAreaMap) {
        return {};
    }
    return _hitAreaMap->get()->pick(pos);
}

} // namespace Render
} // namespace Lattice
