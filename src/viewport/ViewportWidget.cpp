#include "ViewportWidget.hpp"
#include "ViewportRenderer.hpp"
#include "CameraController.hpp"
#include <QOpenGLDebugLogger>

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

#ifdef Q_OS_WIN
    glm::vec2 logicalSize = physicalSize / (logicalDpiX() / 96.f);
#else
    glm::vec2 logicalSize = physicalSize;
#endif
    _renderer->resize(physicalSize, logicalSize);
}

void ViewportWidget::paintGL() {
    _renderer->render();
}

void ViewportWidget::mousePressEvent(QMouseEvent *event) {
    _cameraController.mousePress(event);
}

void ViewportWidget::mouseMoveEvent(QMouseEvent *event) {
    _cameraController.mouseMove(event);
}

void ViewportWidget::mouseReleaseEvent(QMouseEvent *event) {
    _cameraController.mouseRelease(event);
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

} // namespace Lattice
