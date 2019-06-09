#pragma once

#include "MouseEvent.hpp"
#include "../support/SharedPointer.hpp"
#include "../draw/Operations.hpp"
#include <QObject>
#include <glm/glm.hpp>

namespace Lattice {

namespace Draw {
class Operations;
}

namespace Viewport {

class Renderable : public QObject, public EnableSharedFromThis<Renderable> {
    Q_OBJECT
public:
    Renderable() {}
    virtual ~Renderable();

    bool isVisible() const { return _isVisible; }
    void setVisible(bool visible);

    auto& childRenderables() const { return _childRenderables; }
    void setChildRenderables(const std::vector<SP<Renderable>>& children);

    void preDrawRecursive(const SP<Draw::Operations>& operations, const Camera& camera);
    void drawRecursive(const SP<Draw::Operations>& operations, const Camera& camera);
    void drawHitAreaRecursive(const SP<Draw::Operations>& operations, const Camera& camera);
    void draw2DRecursive(QPainter* painter, const QSize& viewportSize);

    void getDescendants(std::vector<SP<Renderable>>& descendants);

    virtual void preDraw(const SP<Draw::Operations>& operations, const Camera& camera);
    virtual void draw(const SP<Draw::Operations>& operations, const Camera& camera);
    virtual void drawHitArea(const SP<Draw::Operations>& operations, const Camera& camera);
    virtual void draw2D(QPainter* painter, const QSize& viewportSize);

    virtual void mousePressEvent(const MouseEvent& event);
    virtual void mouseMoveEvent(const MouseEvent& event);
    virtual void mouseReleaseEvent(const MouseEvent& event);
    virtual void mouseDoubleClickEvent(const MouseEvent& event);
    virtual void contextMenuEvent(const MouseEvent& event);
    virtual void hoverEnterEvent(const MouseEvent& event);
    virtual void hoverLeaveEvent();

    glm::vec4 toIDColor() const;
    static Opt<SP<Renderable>> fromIDColor(glm::vec4 color);

signals:
    void updated();

private:
    std::vector<SP<Renderable>> _childRenderables;
    bool _isVisible = true;
};

} // namespace Render
} // namespace Lattice
