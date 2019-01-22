#include "ViewportContainer.hpp"
#include "Viewport.hpp"
#include "Renderable.hpp"
#include "../support/Debug.hpp"
#include <QMouseEvent>
#include <QOpenGLDebugLogger>

namespace Lattice {
namespace Render {

ViewportContainer::ViewportContainer(QWidget *parent) :
    QOpenGLWidget(parent)
{
}

glm::dvec2 ViewportContainer::mapQtToGL(const QPoint &p) const {
    return glm::dvec2(p.x(), height() - p.y()) / widgetPixelRatio();
}

void ViewportContainer::initializeGL() {
    initializeOpenGLFunctions();

    auto logger = new QOpenGLDebugLogger(this);
    if (logger->initialize()) {
        connect(logger, &QOpenGLDebugLogger::messageLogged, [](const auto& message) {
            qWarning() << message;
        });
        logger->startLogging();
        qDebug() << "OpenGL debug enabled";
    }

    _operations = makeShared<Operations>();
    emit initialized();
}

void ViewportContainer::resizeGL(int w, int h) {
    Q_UNUSED(w); Q_UNUSED(h);
    emit resized();
}

void ViewportContainer::paintGL() {
    emit aboutToBePainted();

    LATTICE_OPTIONAL_GUARD(operations, _operations, return;)

    for (auto child : children()) {
        auto viewport = qobject_cast<Viewport*>(child);
        if (!viewport) { continue; }

        glm::dvec2 minPos = mapQtToGL(viewport->pos() + viewport->rect().bottomLeft());
        glm::dvec2 maxPos = mapQtToGL(viewport->pos() + viewport->rect().topRight());
        glm::ivec2 minPosViewport = round(minPos * (widgetPixelRatio() * devicePixelRatioF()));
        glm::ivec2 maxPosViewport = round(maxPos * (widgetPixelRatio() * devicePixelRatioF()));
        glm::ivec2 sizeViewport = maxPosViewport - minPosViewport;

        glViewport(minPosViewport.x, minPosViewport.y, sizeViewport.x, sizeViewport.y);

        viewport->render(operations);
    }
}

double ViewportContainer::widgetPixelRatio() const {
#ifdef Q_OS_WIN
    return logicalDpiX() / 96.0;
#else
    return 1.0;
#endif
}

} // namespace Renderer
} // namespace Lattice
