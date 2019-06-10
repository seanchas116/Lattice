#pragma once

#include "../support/SharedPointer.hpp"
#include "../support/Camera.hpp"
#include "../draw/Operations.hpp"
#include <QObject>
#include <glm/glm.hpp>

class QMouseEvent;
class QContextMenuEvent;

namespace Lattice {

namespace Draw {
class Operations;
}

namespace Viewport {

struct DrawEvent {
    SP<Draw::Operations> operations;
    Camera camera;
};

struct MouseEvent {
    glm::dvec3 viewportPos;
    Camera camera;
    QMouseEvent* originalMouseEvent;
    QContextMenuEvent* originalContextMenuEvent;

    glm::dvec3 worldPos() const;
};

class Renderable : public QObject, public EnableSharedFromThis<Renderable> {
    Q_OBJECT
public:
    Renderable() {}
    virtual ~Renderable();

    bool isVisible() const { return _isVisible; }
    void setVisible(bool visible);

    auto& childRenderables() const { return _childRenderables; }
    void setChildRenderables(const std::vector<SP<Renderable>>& children);

    void preDrawRecursive(const DrawEvent& event);
    void drawRecursive(const DrawEvent& event);
    void drawHitAreaRecursive(const DrawEvent& event);
    void draw2DRecursive(QPainter* painter, const QSize& viewportSize);

    void getDescendants(std::vector<SP<Renderable>>& descendants);

    virtual void preDraw(const DrawEvent& event);
    virtual void draw(const DrawEvent& event);
    virtual void drawHitArea(const DrawEvent& event);
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
