#include "RenderWidget.hpp"
#include "Renderable.hpp"
#include "../support/Debug.hpp"
#include <QResizeEvent>

namespace Lattice {
namespace Render {

RenderWidget::RenderWidget(QWidget *parent) :
    QOpenGLWidget(parent)
{
}

glm::ivec2 RenderWidget::logicalSize() const {
    return glm::round(glm::dvec2(width(), height()) / widgetPixelRatio());
}

void RenderWidget::resizeEvent(QResizeEvent *event) {
    Q_UNUSED(event)
    glm::ivec2 physicalSize(event->size().width(), event->size().height());
    glm::ivec2 logicalSize = glm::round(glm::dvec2(physicalSize) / widgetPixelRatio());
    _logicalSize = logicalSize;
    emit sizeChanged(logicalSize, physicalSize);
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
    _operations = makeShared<Operations>();
    emit initialized();
}

void RenderWidget::resizeGL(int w, int h) {
    /*
    glm::ivec2 physicalSize(w, h);
    glm::ivec2 logicalSize = glm::round(glm::dvec2(physicalSize) / widgetPixelRatio());
    _logicalSize = logicalSize;
    emit sizeChanged(logicalSize, physicalSize);
    */
    glViewport(0, 0, w, h);
}

void RenderWidget::paintGL() {
    emit aboutToBePainted();

    LATTICE_OPTIONAL_GUARD(operations, _operations, return;)

    for (auto& viewport : _viewports) {
        glm::dvec2 minPos = viewport.offset;
        glm::dvec2 maxPos = minPos + viewport.camera.viewSize();
        glm::ivec2 minPosWidget = round(minPos * widgetPixelRatio());
        glm::ivec2 maxPosWidget = round(maxPos * widgetPixelRatio());
        glm::ivec2 sizeWidget = maxPosWidget - minPosWidget;
        Q_UNUSED(sizeWidget)

        glViewport(minPosWidget.x, minPosWidget.y, sizeWidget.x, sizeWidget.y);
        qDebug() << minPosWidget << sizeWidget;

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
