#include "ViewportWidget.hpp"

namespace Lattice {

ViewportWidget::ViewportWidget(QWidget *parent) : QOpenGLWidget(parent)
{

}

void ViewportWidget::initializeGL() {
    initializeOpenGLFunctions();
}

void ViewportWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

void ViewportWidget::paintGL() {
    glClearColor(1, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}

} // namespace Lattice
