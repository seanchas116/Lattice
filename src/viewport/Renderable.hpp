#pragma once

#include "MouseEvent.hpp"
#include "PickableID.hpp"
#include "../support/SharedPointer.hpp"
#include <glm/glm.hpp>

namespace Lattice {

namespace Draw {
class Operations;
}

namespace Viewport {

class Renderable : public EnableSharedFromThis<Renderable> {
    Q_DISABLE_COPY(Renderable)
public:
    Renderable() {}
    virtual ~Renderable();

    virtual void draw(const SP<Draw::Operations>& operations, const SP<Camera>& camera);
    virtual void drawPickables(const SP<Draw::Operations>& operations, const SP<Camera>& camera, const PickableID& pickableID);
    virtual void draw2D(QPainter* painter, const QSize& viewportSize);

    virtual void mousePressEvent(const MouseEvent& event);
    virtual void mouseMoveEvent(const MouseEvent& event);
    virtual void mouseReleaseEvent(const MouseEvent& event);
    virtual void mouseDoubleClickEvent(const MouseEvent& event);
    virtual void contextMenuEvent(const ContextMenuEvent& event);
    virtual void hoverEnterEvent(const MouseEvent& event);
    virtual void hoverLeaveEvent();
};

} // namespace Render
} // namespace Lattice
