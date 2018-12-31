#include "ViewportWidget.hpp"
#include "ViewportRenderer.hpp"
#include "CameraController.hpp"
#include <QOpenGLDebugLogger>
#include <QMouseEvent>

namespace Lattice {

ViewportWidget::ViewportWidget(const SP<AppState> &appState, QWidget *parent) : QOpenGLWidget(parent)
{
    _appState = appState;
    setFocusPolicy(Qt::ClickFocus);
    connect(&_keyObserver, &KeyObserver::selectedKeysChanged, &_cameraController, &CameraController::setPressedKeys);
}

void ViewportWidget::initializeGL() {
    auto logger = new QOpenGLDebugLogger(this);
    if (logger->initialize()) {
        connect(logger, &QOpenGLDebugLogger::messageLogged, [](const auto& message) {
            qDebug() << message;
        });
        logger->startLogging();
        qDebug() << "OpenGL debug enabled";
    }

    _renderer = std::make_shared<ViewportRenderer>(_appState);
    connect(_renderer.get(), &ViewportRenderer::updateNeeded, this, [this] { update(); });

    _renderer->setCameraLocation(_cameraController.location());
    connect(&_cameraController, &CameraController::locationChanged, this, [this] (const Location& location) {
        _renderer->setCameraLocation(location);
        update();
    });
}

void ViewportWidget::resizeGL(int w, int h) {
    glm::vec2 physicalSize(w, h);
    glm::vec2 logicalSize = physicalSize / float(widgetPixelRatio());
    _renderer->resize(physicalSize, logicalSize);
}

void ViewportWidget::paintGL() {
    _renderer->render();
}

void ViewportWidget::mousePressEvent(QMouseEvent *event) {
    if (_cameraController.mousePress(event)) {
        return;
    }
    _renderer->mousePress(event, mapToRenderer(event->pos()));
}

void ViewportWidget::mouseMoveEvent(QMouseEvent *event) {
    if (_cameraController.mouseMove(event)) {
        return;
    }
    _renderer->mouseMove(event, mapToRenderer(event->pos()));
}

void ViewportWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (_cameraController.mouseRelease(event)) {
        return;
    }
    _renderer->mouseRelease(event, mapToRenderer(event->pos()));
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

glm::vec2 ViewportWidget::mapToRenderer(const QPoint &p) {
    return glm::vec2(p.x(), height() - p.y()) / float(widgetPixelRatio());
}

} // namespace Lattice
