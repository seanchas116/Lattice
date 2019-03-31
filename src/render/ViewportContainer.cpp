#include "ViewportContainer.hpp"
#include "Viewport.hpp"
#include "RenderableObject.hpp"
#include "Util.hpp"
#include "PickableMap.hpp"
#include "../support/Debug.hpp"
#include <QMouseEvent>
#include <QOpenGLDebugLogger>
#include <QOpenGLPaintDevice>
#include <QPainter>

namespace Lattice {
namespace Render {

ViewportContainer::ViewportContainer(QWidget *parent) :
    QOpenGLWidget(parent)
{
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
        connect(logger, &QOpenGLDebugLogger::messageLogged, [](const QOpenGLDebugMessage& message) {
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

    for (auto viewport : _viewports) {
        if (!viewport->_renderable) {
            continue;
        }
        glm::dvec2 minPos = mapQtToGL(this, viewport->mapTo(this, viewport->rect().bottomLeft()));
        glm::dvec2 maxPos = mapQtToGL(this, viewport->mapTo(this, viewport->rect().topRight()));
        glm::ivec2 minPosViewport = round(minPos * devicePixelRatioF());
        glm::ivec2 maxPosViewport = round(maxPos * devicePixelRatioF());
        glm::ivec2 sizeViewport = maxPosViewport - minPosViewport;

        glEnable(GL_SCISSOR_TEST);
        glScissor(minPosViewport.x, minPosViewport.y, sizeViewport.x, sizeViewport.y);
        glViewport(minPosViewport.x, minPosViewport.y, sizeViewport.x, sizeViewport.y);
        glBindFramebuffer(GL_FRAMEBUFFER, QOpenGLContext::currentContext()->defaultFramebufferObject());

        (*viewport->_renderable)->drawRecursive(operations, viewport->_camera);

        glDisable(GL_SCISSOR_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        viewport->pickableMap()->draw(*viewport->_renderable, operations, viewport->camera());
    }

    //glDisable(GL_SCISSOR_TEST);
    //glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    //glViewport(0, 0, width() * devicePixelRatio(), height() * devicePixelRatio());
    //glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());

    QOpenGLPaintDevice device(width() * devicePixelRatio(), height() * devicePixelRatio());
    QPainter painter(&device);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::blue);
    painter.drawRect(0, 0, 100, 100);
}

} // namespace Renderer
} // namespace Lattice
