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
    initializeOpenGLFunctions();
}

void RenderWidget::resizeGL(int w, int h) {
    Q_UNUSED(w); Q_UNUSED(h);
}

void RenderWidget::paintGL() {
    for (auto& viewport : _viewports) {
        glm::dvec2 minPos = viewport.offset;
        glm::dvec2 maxPos = minPos + viewport.camera.viewSize();
        glm::ivec2 minPosWidget = round(minPos * widgetPixelRatio());
        glm::ivec2 maxPosWidget = round(maxPos * widgetPixelRatio());
        glm::ivec2 sizeWidget = maxPosWidget - minPosWidget;

        glViewport(minPosWidget.x, minPosWidget.y, sizeWidget.x, sizeWidget.y);

        for (auto& renderable : _renderables) {
            renderable->draw(_operations.sharedFromThis(), viewport.camera);
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
