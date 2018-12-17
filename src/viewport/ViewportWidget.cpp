#include "ViewportWidget.hpp"
#include "ViewportRenderer.hpp"
#include <QOpenGLDebugLogger>

namespace Lattice {

ViewportWidget::ViewportWidget(QWidget *parent) : QOpenGLWidget(parent)
{
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
}

void ViewportWidget::resizeGL(int w, int h) {
    _renderer->resize({w, h});
}

void ViewportWidget::paintGL() {
    _renderer->render();
}

} // namespace Lattice
