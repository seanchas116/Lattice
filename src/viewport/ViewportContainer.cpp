#include "ViewportContainer.hpp"
#include "../support/Debug.hpp"
#include "HitAreaMap.hpp"
#include "Renderable.hpp"
#include "Util.hpp"
#include "Viewport.hpp"
#include <QMouseEvent>
#include <QOpenGLDebugLogger>
#include <QOpenGLPaintDevice>
#include <QPainter>
#include <QTransform>

namespace Lattice {
namespace Viewport {

ViewportContainer::ViewportContainer(QWidget *parent) : QOpenGLWidget(parent) {
}

void ViewportContainer::setViewports(const std::vector<Viewport *> &viewports) {
    _viewports = viewports;
    for (auto v : viewports) {
        connect(v, &Viewport::updateRequested, this, [this] { update(); });
    }
}

void ViewportContainer::initializeGL() {
    initializeOpenGLFunctions();

    auto logger = new QOpenGLDebugLogger(this);
    if (logger->initialize()) {
        connect(logger, &QOpenGLDebugLogger::messageLogged, [](const QOpenGLDebugMessage &message) {
            if (message.severity() == QOpenGLDebugMessage::NotificationSeverity) {
                return;
            }
            qWarning() << message.message();
        });
        logger->startLogging();
        qDebug() << "OpenGL debug enabled";
    }

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    _operations = makeShared<Draw::Operations>();
    emit initialized();
}

void ViewportContainer::resizeGL(int w, int h) {
    Q_UNUSED(w);
    Q_UNUSED(h);
    emit resized();
}

void ViewportContainer::paintGL() {
    emit aboutToBePainted();

    LATTICE_OPTIONAL_GUARD(operations, _operations, return;)

    for (auto viewport : _viewports) {
        if (!viewport->_renderable) {
            continue;
        }

        DrawEvent drawEvent{viewport, viewport->camera(), operations};

        (*viewport->_renderable)->preDrawRecursive(drawEvent);

        glm::dvec2 minPos = mapQtToGL(this, viewport->mapTo(this, viewport->rect().bottomLeft()));
        glm::dvec2 maxPos = mapQtToGL(this, viewport->mapTo(this, viewport->rect().topRight()));
        glm::ivec2 minPosViewport = round(minPos * devicePixelRatioF());
        glm::ivec2 maxPosViewport = round(maxPos * devicePixelRatioF());
        glm::ivec2 sizeViewport = maxPosViewport - minPosViewport;

        glEnable(GL_SCISSOR_TEST);
        glScissor(minPosViewport.x, minPosViewport.y, sizeViewport.x, sizeViewport.y);
        glViewport(minPosViewport.x, minPosViewport.y, sizeViewport.x, sizeViewport.y);
        glBindFramebuffer(GL_FRAMEBUFFER, QOpenGLContext::currentContext()->defaultFramebufferObject());

        (*viewport->_renderable)->drawRecursive(drawEvent);

        glDisable(GL_SCISSOR_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        viewport->hitAreaMap()->draw(*viewport->_renderable, drawEvent);
    }

    glDisable(GL_CULL_FACE);
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());

    QOpenGLPaintDevice device(width() * devicePixelRatioF(), height() * devicePixelRatioF());
    QPainter painter(&device);
    painter.setRenderHint(QPainter::Antialiasing);

    for (auto viewport : _viewports) {
        if (!viewport->_renderable) {
            continue;
        }
        painter.save();
        auto offset = viewport->mapTo(this, viewport->rect().topLeft());
        auto transform = QTransform(1, 0, 0, -1, 0, viewport->rect().height()) * QTransform::fromTranslate(offset.x(), offset.y()) * QTransform::fromScale(devicePixelRatioF(), devicePixelRatioF());
        painter.setTransform(transform);
        painter.setClipRect(0, 0, viewport->width(), viewport->height());

        Draw2DEvent event{viewport, viewport->size(), &painter};
        (*viewport->_renderable)->draw2DRecursive(event);
        painter.restore();
    }
}

} // namespace Viewport
} // namespace Lattice
