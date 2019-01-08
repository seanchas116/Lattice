#include "RenderWidget.hpp"
#include "Renderable.hpp"

namespace Lattice {
namespace Renderer {

RenderWidget::RenderWidget(QWidget *parent) :
    QOpenGLWidget(parent)
{
}

void RenderWidget::mousePressEvent(QMouseEvent *event) {
    Q_UNUSED(event);
}

void RenderWidget::mouseReleaseEvent(QMouseEvent *event) {
    Q_UNUSED(event);
}

void RenderWidget::mouseMoveEvent(QMouseEvent *event) {
    Q_UNUSED(event);
}

void RenderWidget::initializeGL() {
}

void RenderWidget::resizeGL(int w, int h) {
    Q_UNUSED(w); Q_UNUSED(h);
}

void RenderWidget::paintGL() {
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
