#pragma once

#include "MouseEvent.hpp"
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
    virtual void drawPickables(const SP<Draw::Operations>& operations, const SP<Camera>& camera);
    virtual void draw2D(QPainter* painter, glm::ivec2 viewportSize);

    virtual void mousePressEvent(const MouseEvent& event);
    virtual void mouseMoveEvent(const MouseEvent& event);
    virtual void mouseReleaseEvent(const MouseEvent& event);
    virtual void mouseDoubleClickEvent(const MouseEvent& event);
    virtual void contextMenuEvent(const ContextMenuEvent& event);
    virtual void hoverEnterEvent(const MouseEvent& event);
    virtual void hoverLeaveEvent();

    glm::vec4 toIDColor() const;
    static Opt<SP<Renderable>> fromIDColor(glm::vec4 color);
};

} // namespace Render
} // namespace Lattice
