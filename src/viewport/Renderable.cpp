#include "Renderable.hpp"

namespace Lattice {
namespace Viewport {

glm::dvec3 MouseEvent::worldPos() const {
    return camera.mapViewportToWorld(viewportPos);
}

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

void Renderable::preDrawRecursive(const DrawEvent& event) {
    if (!_isVisible) {
        return;
    }
    preDraw(event);
    for (auto& c : childRenderables()) {
        c->preDrawRecursive(event);
    }
}

void Renderable::drawRecursive(const DrawEvent &event) {
    if (!_isVisible) {
        return;
    }
    draw(event);
    for (auto& c : childRenderables()) {
        c->drawRecursive(event);
    }
}

void Renderable::drawHitAreaRecursive(const DrawEvent &event) {
    if (!_isVisible) {
        return;
    }
    drawHitArea(event);
    for (auto& c : childRenderables()) {
        c->drawHitAreaRecursive(event);
    }
}

void Renderable::draw2DRecursive(const Draw2DEvent &event) {
    if (!_isVisible) {
        return;
    }
    draw2D(event);
    for (auto& c : childRenderables()) {
        c->draw2DRecursive(event);
    }
}

void Renderable::getDescendants(std::vector<SP<Renderable>> &descendants) {
    descendants.push_back(sharedFromThis());
    for (auto& c : _childRenderables) {
        c->getDescendants(descendants);
    }
}

void Renderable::preDraw(const DrawEvent &event) {
    Q_UNUSED(event);
}

void Renderable::draw(const DrawEvent &event) {
    Q_UNUSED(event);
}

void Renderable::drawHitArea(const DrawEvent &event) {
    Q_UNUSED(event);
}

void Renderable::draw2D(const Draw2DEvent &event) {
    Q_UNUSED(event);
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
