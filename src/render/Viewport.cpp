#include "Viewport.hpp"
#include "Renderable.hpp"
#include "Util.hpp"
#include "../support/Debug.hpp"
#include <QMouseEvent>
#include <QOpenGLDebugLogger>
#include <QResizeEvent>

namespace Lattice {
namespace Render {

Viewport::Viewport(QWidget *parent) : QWidget(parent), _camera(makeShared<Camera>()) {
    connect(_camera.get(), &Camera::changed, this, &Viewport::updateRequested);
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
    LATTICE_OPTIONAL_GUARD(renderable, _draggedRenderable, return;)
    MouseEvent renderMouseEvent(event, mapQtToGL(this, event->pos()), _camera, _hitResult);
    renderable->mouseMove(renderMouseEvent);
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

std::optional<std::pair<SP<Renderable>, HitResult> > Viewport::hitTest(glm::dvec2 pos, const SP<Camera> &camera) {
    std::map<double, std::pair<SP<Renderable>, HitResult>> hitRenderables;
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

void Viewport::render(const SP<Operations> &operations) {
    for (auto& renderable : _renderables) {
        renderable->draw(operations, _camera);
    }
}

void Viewport::setCameraLocation(const Location &location) {
    _camera->setLocation(location);
    emit updateRequested();
}

void Viewport::setCameraProjection(const Camera::Projection &projection) {
    _camera->setProjection(projection);
    emit updateRequested();
}

} // namespace Render
} // namespace Lattice
