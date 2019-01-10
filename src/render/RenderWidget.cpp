#include "RenderWidget.hpp"
#include "Renderable.hpp"
#include "../support/Debug.hpp"
#include <QMouseEvent>
#include <QOpenGLDebugLogger>

namespace Lattice {
namespace Render {

RenderWidget::RenderWidget(QWidget *parent) :
    QOpenGLWidget(parent)
{
}

glm::ivec2 RenderWidget::logicalSize() const {
    return glm::round(glm::dvec2(width(), height()) / widgetPixelRatio());
}

glm::dvec2 RenderWidget::mapQtToGL(const QPoint &p) const {
    return glm::dvec2(p.x(), height() - p.y()) / widgetPixelRatio();
}

void RenderWidget::mousePressEvent(QMouseEvent *event) {
    auto pos = mapQtToGL(event->pos());

    auto& camera = _viewports[0].camera; // FIXME

    for (auto it = _layers.rbegin(); it != _layers.rend(); ++it) {
        auto& layer = *it;
        std::map<double, std::pair<SP<Renderable>, HitResult>> hitRenderables;
        for (auto& renderable : layer) {
            auto maybeHitResult = renderable->hitTest(pos, camera);
            if (maybeHitResult) {
                hitRenderables.insert({maybeHitResult->t, {renderable, *maybeHitResult}});
            }
        }
        if (!hitRenderables.empty()) {
            auto [renderable, hitResult] = hitRenderables.begin()->second;
            renderable->mousePress(event, pos, camera, hitResult);
            _draggedRenderable = renderable;
            break;
        }
    }
}

void RenderWidget::mouseReleaseEvent(QMouseEvent *event) {
    LATTICE_OPTIONAL_GUARD(renderable, _draggedRenderable, return;)
    renderable->mouseRelease(event, mapQtToGL(event->pos()), _viewports[_draggedViewportIndex].camera);
    _draggedRenderable = {};
}

void RenderWidget::mouseMoveEvent(QMouseEvent *event) {
    LATTICE_OPTIONAL_GUARD(renderable, _draggedRenderable, return;)
    renderable->mouseMove(event, mapQtToGL(event->pos()), _viewports[_draggedViewportIndex].camera);
}

void RenderWidget::initializeGL() {
    initializeOpenGLFunctions();

    auto logger = new QOpenGLDebugLogger(this);
    if (logger->initialize()) {
        connect(logger, &QOpenGLDebugLogger::messageLogged, [](const auto& message) {
            qWarning() << message;
        });
        logger->startLogging();
        qDebug() << "OpenGL debug enabled";
    }

    _operations = makeShared<Operations>();
    emit initialized();
}

void RenderWidget::resizeGL(int w, int h) {
    Q_UNUSED(w); Q_UNUSED(h);
    emit resized();
}

void RenderWidget::paintGL() {
    emit aboutToBePainted();

    LATTICE_OPTIONAL_GUARD(operations, _operations, return;)

    for (auto& viewport : _viewports) {
        glm::dvec2 minPos = viewport.offset;
        glm::dvec2 maxPos = minPos + viewport.camera.viewSize();
        glm::ivec2 minPosWidget = round(minPos * (widgetPixelRatio() * devicePixelRatioF()));
        glm::ivec2 maxPosWidget = round(maxPos * (widgetPixelRatio() * devicePixelRatioF()));
        glm::ivec2 sizeWidget = maxPosWidget - minPosWidget;

        glViewport(minPosWidget.x, minPosWidget.y, sizeWidget.x, sizeWidget.y);

        for (auto& layer : _layers) {
            for (auto& renderable : layer) {
                renderable->draw(operations, viewport.camera);
            }
        }
    }
}

double RenderWidget::widgetPixelRatio() const {
#ifdef Q_OS_WIN
    return logicalDpiX() / 96.0;
#else
    return 1.0;
#endif
}

} // namespace Renderer
} // namespace Lattice
