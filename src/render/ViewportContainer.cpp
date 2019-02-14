#include "ViewportContainer.hpp"
#include "Viewport.hpp"
#include "Renderable.hpp"
#include "Util.hpp"
#include "PickableMap.hpp"
#include "../support/Debug.hpp"
#include <QMouseEvent>
#include <QOpenGLDebugLogger>

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

    for (auto viewport : _viewports) {
        glm::dvec2 minPos = mapQtToGL(this, viewport->mapTo(this, viewport->rect().bottomLeft()));
        glm::dvec2 maxPos = mapQtToGL(this, viewport->mapTo(this, viewport->rect().topRight()));
        glm::ivec2 minPosViewport = round(minPos * (widgetPixelRatio(this) * devicePixelRatioF()));
        glm::ivec2 maxPosViewport = round(maxPos * (widgetPixelRatio(this) * devicePixelRatioF()));
        glm::ivec2 sizeViewport = maxPosViewport - minPosViewport;

        glEnable(GL_SCISSOR_TEST);
        glScissor(minPosViewport.x, minPosViewport.y, sizeViewport.x, sizeViewport.y);
        glViewport(minPosViewport.x, minPosViewport.y, sizeViewport.x, sizeViewport.y);
        glBindFramebuffer(GL_FRAMEBUFFER, QOpenGLContext::currentContext()->defaultFramebufferObject());

        for (auto& renderable : viewport->renderables()) {
            renderable->draw(operations, viewport->camera());
        }

        glDisable(GL_SCISSOR_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        viewport->pickableMap()->draw(viewport->renderables(), operations, viewport->camera());
    }
}

} // namespace Renderer
} // namespace Lattice
