#pragma once
#include "MouseEvent.hpp"
#include "HitResult.hpp"
#include "../draw/Operations.hpp"
#include "../support/Shorthands.hpp"
#include "../support/Camera.hpp"
#include <QOpenGLWidget>

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
    void setCamera(const Camera& camera);

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

    Opt<HitResult> hitTest(glm::dvec2 pos, const Camera& camera);

    Opt<SP<Renderable>> _renderable;
    Opt<HitResult> _draggedHitResult;
    Opt<HitResult> _hoveredHitResult;
    Opt<SP<HitAreaMap>> _hitAreaMap;
    Camera _camera;
};

} // namespace Renderer
} // namespace Lattice
