#include "ViewportWidget.hpp"
#include "ViewportRenderer.hpp"
#include "CameraController.hpp"
#include <QOpenGLDebugLogger>
#include <QMouseEvent>

namespace Lattice::Viewport {

ViewportWidget::ViewportWidget(const SP<UI::AppState> &appState, QWidget *parent) :
    QOpenGLWidget(parent),
    _appState(appState)
{
    setFocusPolicy(Qt::ClickFocus);
    connect(&_keyObserver, &KeyObserver::selectedKeysChanged, &_cameraController, &CameraController::setPressedKeys);
}

void ViewportWidget::initializeGL() {
    auto logger = new QOpenGLDebugLogger(this);
    if (logger->initialize()) {
        connect(logger, &QOpenGLDebugLogger::messageLogged, [](const auto& message) {
            qWarning() << message;
        });
        logger->startLogging();
        qDebug() << "OpenGL debug enabled";
    }

    auto renderer = makeShared<ViewportRenderer>(_appState);
    _renderer = renderer;
    connect(renderer.get(), &ViewportRenderer::updateNeeded, this, [this] { update(); });

    renderer->setCameraLocation(_cameraController.location());
    connect(&_cameraController, &CameraController::locationChanged, this, [this] (const Location& location) {
        LATTICE_OPTIONAL_GUARD(renderer, _renderer, return;)
        renderer->setCameraLocation(location);
        update();
    });
}

void ViewportWidget::resizeGL(int w, int h) {
    LATTICE_OPTIONAL_GUARD(renderer, _renderer, return;)
    glm::vec2 physicalSize(w, h);
    glm::vec2 logicalSize = physicalSize / float(widgetPixelRatio());
    renderer->resize(physicalSize, logicalSize);
}

void ViewportWidget::paintGL() {
    LATTICE_OPTIONAL_GUARD(renderer, _renderer, return;)
    renderer->render();
}

void ViewportWidget::mousePressEvent(QMouseEvent *event) {
    LATTICE_OPTIONAL_GUARD(renderer, _renderer, return;)
    if (_cameraController.mousePress(event)) {
        return;
    }
    renderer->mousePress(event, mapToRenderer(event->pos()));
}

void ViewportWidget::mouseMoveEvent(QMouseEvent *event) {
    LATTICE_OPTIONAL_GUARD(renderer, _renderer, return;)
    if (_cameraController.mouseMove(event)) {
        return;
    }
    renderer->mouseMove(event, mapToRenderer(event->pos()));
}

void ViewportWidget::mouseReleaseEvent(QMouseEvent *event) {
    LATTICE_OPTIONAL_GUARD(renderer, _renderer, return;)
    if (_cameraController.mouseRelease(event)) {
        return;
    }
    renderer->mouseRelease(event, mapToRenderer(event->pos()));
}

void ViewportWidget::wheelEvent(QWheelEvent *event) {
    _cameraController.wheel(event);
}

void ViewportWidget::keyPressEvent(QKeyEvent *event) {
    _keyObserver.keyPress(event);
}

void ViewportWidget::keyReleaseEvent(QKeyEvent *event) {
    _keyObserver.keyRelease(event);
}

double ViewportWidget::widgetPixelRatio() const {
#ifdef Q_OS_WIN
    return logicalDpiX() / 96.0;
#else
    return 1.0;
#endif
}

glm::dvec2 ViewportWidget::mapToRenderer(const QPoint &p) {
    return glm::dvec2(p.x(), height() - p.y()) / widgetPixelRatio();
}

} // namespace Lattice
