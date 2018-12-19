#include "ViewportWidget.hpp"
#include "ViewportRenderer.hpp"
#include "CameraController.hpp"
#include <QOpenGLDebugLogger>

namespace Lattice {

ViewportWidget::ViewportWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    _cameraController = std::make_shared<CameraController>();
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

    _renderer = std::make_shared<ViewportRenderer>();
    connect(_cameraController.get(), &CameraController::cameraChanged, this, [this] (const Camera& camera) {
        _renderer->setCamera(camera);
        update();
    });
}

void ViewportWidget::resizeGL(int w, int h) {
    _renderer->resize({w, h});
}

void ViewportWidget::paintGL() {
    _renderer->render();
}

void ViewportWidget::mousePressEvent(QMouseEvent *event) {
    _cameraController->mousePress(event);
}

void ViewportWidget::mouseMoveEvent(QMouseEvent *event) {
    _cameraController->mouseMove(event);
}

void ViewportWidget::mouseReleaseEvent(QMouseEvent *event) {
    _cameraController->mouseRelease(event);
}

} // namespace Lattice
