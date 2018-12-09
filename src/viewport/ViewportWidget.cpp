#include "ViewportWidget.hpp"
#include "ViewportRenderer.hpp"

namespace Lattice {

ViewportWidget::ViewportWidget(QWidget *parent) : QOpenGLWidget(parent)
{
}

void ViewportWidget::initializeGL() {
    _renderer = std::make_shared<ViewportRenderer>();
}

void ViewportWidget::resizeGL(int w, int h) {
    _renderer->resize({w, h});
}

void ViewportWidget::paintGL() {
    _renderer->render();
}

} // namespace Lattice
