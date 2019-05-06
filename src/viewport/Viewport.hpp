#pragma once
#include <QOpenGLWidget>
#include "../support/Shorthands.hpp"
#include "../draw/Operations.hpp"
#include "MouseEvent.hpp"

namespace Lattice {
namespace Viewport {

class Renderable;
class RenderableObject;
class RenderWidget;
class HitAreaMap;

class Viewport : public QWidget {
    Q_OBJECT
    using super = QWidget;
public:
    Viewport(QWidget* parent = nullptr);

    void setRenderable(const Opt<SP<Renderable> > &renderable);

    auto& camera() const { return _camera; }

signals:
    void updateRequested();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    void moveEvent(QMoveEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    friend class ViewportContainer;

    const SP<HitAreaMap>& hitAreaMap();

    Opt<std::pair<SP<Renderable>, double>> hitTest(glm::dvec2 pos, const SP<Camera>& camera);

    Opt<SP<Renderable>> _renderable;
    Opt<SP<Renderable>> _draggedRenderable;
    Opt<SP<Renderable>> _hoveredRenderable;
    Opt<SP<HitAreaMap>> _hitAreaMap;
    double _hitDepth;
    SP<Camera> _camera;
};

} // namespace Renderer
} // namespace Lattice
