#include "RenderableObject.hpp"

namespace Lattice {
namespace Render {

void RenderableObject::setVisible(bool visible) {
    if (_isVisible == visible) {
        return;
    }
    _isVisible = visible;
    update();
}

void RenderableObject::drawRecursive(const SP<Operations> &operations, const SP<Camera> &camera) {
    if (!_isVisible) {
        return;
    }
    draw(operations, camera);
    for (auto& c : childRenderables()) {
        auto childObj = dynamicPointerCast<RenderableObject>(c);
        if (childObj) {
            (*childObj)->drawRecursive(operations, camera);
        } else {
            c->draw(operations, camera);
        }
    }
}

void RenderableObject::drawPickablesRecursive(const SP<Operations> &operations, const SP<Camera> &camera, std::vector<SP<Renderable> > &renderedChildren) {
    if (!_isVisible) {
        return;
    }
    drawPickables(operations, camera);
    for (auto& c : childRenderables()) {
        auto childObj = dynamicPointerCast<RenderableObject>(c);
        if (childObj) {
            (*childObj)->drawPickablesRecursive(operations, camera, renderedChildren);
        } else {
            c->drawPickables(operations, camera);
        }
    }
    renderedChildren.insert(renderedChildren.end(), _childRenderables.begin(), _childRenderables.end());
}

void RenderableObject::setChildRenderables(const std::vector<SP<Renderable> > &children) {
    for (auto& child : _childRenderables) {
        auto childObj = dynamicPointerCast<RenderableObject>(child);
        if (childObj) {
            disconnect(childObj->get(), &RenderableObject::updated, this, &RenderableObject::updated);
        }
    }

    _childRenderables = children;

    for (auto& child : _childRenderables) {
        auto childObj = dynamicPointerCast<RenderableObject>(child);
        if (childObj) {
            connect(childObj->get(), &RenderableObject::updated, this, &RenderableObject::updated);
        }
    }

    update();
}

} // namespace Renderer
} // namespace Lattice
