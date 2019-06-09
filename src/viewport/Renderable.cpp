#include "Renderable.hpp"

namespace Lattice {
namespace Viewport {

Renderable::~Renderable() {
}

void Renderable::setVisible(bool visible) {
    if (_isVisible == visible) {
        return;
    }
    _isVisible = visible;
    emit updated();
}

void Renderable::setChildRenderables(const std::vector<SP<Renderable> > &children) {
    for (auto& child : _childRenderables) {
        disconnect(child.get(), &Renderable::updated, this, &Renderable::updated);
    }
    _childRenderables = children;
    for (auto& child : _childRenderables) {
        connect(child.get(), &Renderable::updated, this, &Renderable::updated);
    }
    emit updated();
}

void Renderable::preDrawRecursive(const SP<Draw::Operations> &operations, const SP<OldCamera> &camera) {
    if (!_isVisible) {
        return;
    }
    preDraw(operations, camera);
    for (auto& c : childRenderables()) {
        c->preDrawRecursive(operations, camera);
    }
}

void Renderable::drawRecursive(const SP<Draw::Operations> &operations, const SP<OldCamera> &camera) {
    if (!_isVisible) {
        return;
    }
    draw(operations, camera);
    for (auto& c : childRenderables()) {
        c->drawRecursive(operations, camera);
    }
}

void Renderable::drawHitAreaRecursive(const SP<Draw::Operations> &operations, const SP<OldCamera> &camera) {
    if (!_isVisible) {
        return;
    }
    drawHitArea(operations, camera);
    for (auto& c : childRenderables()) {
        c->drawHitAreaRecursive(operations, camera);
    }
}

void Renderable::draw2DRecursive(QPainter *painter, const QSize &viewportSize) {
    if (!_isVisible) {
        return;
    }
    draw2D(painter, viewportSize);
    for (auto& c : childRenderables()) {
        c->draw2DRecursive(painter, viewportSize);
    }
}

void Renderable::getDescendants(std::vector<SP<Renderable>> &descendants) {
    descendants.push_back(sharedFromThis());
    for (auto& c : _childRenderables) {
        c->getDescendants(descendants);
    }
}

void Renderable::preDraw(const SP<Draw::Operations> &operations, const SP<OldCamera> &camera) {
    Q_UNUSED(operations); Q_UNUSED(camera);
}

void Renderable::draw(const SP<Draw::Operations> &operations, const SP<OldCamera> &camera) {
    Q_UNUSED(operations); Q_UNUSED(camera);
}

void Renderable::drawHitArea(const SP<Draw::Operations> &operations, const SP<OldCamera> &camera) {
    Q_UNUSED(operations); Q_UNUSED(camera);
}

void Renderable::draw2D(QPainter *painter, const QSize &viewportSize) {
    Q_UNUSED(painter); Q_UNUSED(viewportSize);
}

void Renderable::mousePressEvent(const MouseEvent &event) {
    Q_UNUSED(event);
}

void Renderable::mouseMoveEvent(const MouseEvent &event) {
    Q_UNUSED(event);
}

void Renderable::mouseReleaseEvent(const MouseEvent &event) {
    Q_UNUSED(event);
}

void Renderable::mouseDoubleClickEvent(const MouseEvent &event) {
    Q_UNUSED(event);
}

void Renderable::contextMenuEvent(const MouseEvent &event) {
    Q_UNUSED(event);
}

void Renderable::hoverEnterEvent(const MouseEvent &event) {
    Q_UNUSED(event);
}

void Renderable::hoverLeaveEvent() {
}

glm::vec4 Renderable::toIDColor() const {
    union {
        const Renderable* ptr;
        glm::u16vec4 color;
    } idColor;
    idColor.ptr = this;

    return glm::vec4(idColor.color) / float(0xFFFF);
}

Opt<SP<Renderable> > Renderable::fromIDColor(glm::vec4 color) {
    union {
        Renderable* ptr;
        glm::u16vec4 color;
    } idColor;
    idColor.color = glm::u16vec4(glm::round(color * float(0xFFFF)));
    if (!idColor.ptr) {
        return {};
    }
    return idColor.ptr->sharedFromThis();
}

} // namespace Render
} // namespace Lattice
