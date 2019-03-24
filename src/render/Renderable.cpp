#include "Renderable.hpp"

namespace Lattice {
namespace Render {

Renderable::~Renderable() {
}

void Renderable::draw(const SP<Operations> &operations, const SP<Camera> &camera) {
    Q_UNUSED(operations); Q_UNUSED(camera);
}

void Renderable::drawPickables(const SP<Operations> &operations, const SP<Camera> &camera) {
    Q_UNUSED(operations); Q_UNUSED(camera);
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

void Renderable::contextMenuEvent(const ContextMenuEvent &event) {
    Q_UNUSED(event);
}

void Renderable::hoverEnterEvent(const MouseEvent &event) {
    Q_UNUSED(event);
}

void Renderable::hoverLeaveEvent() {
}

void Renderable::drawRecursive(const SP<Operations> &operations, const SP<Camera> &camera) {
    draw(operations, camera);
    for (auto& c : children()) {
        c->drawRecursive(operations, camera);
    }
}

void Renderable::drawPickablesRecursive(const SP<Operations> &operations, const SP<Camera> &camera, std::vector<SP<Renderable> > &renderedChildren) {
    drawPickables(operations, camera);
    for (auto& c : children()) {
        c->drawPickablesRecursive(operations, camera, renderedChildren);
    }
    renderedChildren.insert(renderedChildren.end(), _children.begin(), _children.end());
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

void Renderable::setChildren(const std::vector<SP<Renderable> > &children) {
    for (auto& c : _children) {
        c->_parent = nullptr;
    }
    _children = children;
    for (auto& c : children) {
        c->_parent = this;
    }
    update();
}

void Renderable::update() {
    onUpdate();
    if (_parent) {
        _parent->update();
    }
}

void Renderable::onUpdate() {
}

} // namespace Render
} // namespace Lattice
