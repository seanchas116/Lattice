#pragma once
#include <QOpenGLWidget>
#include "../support/Shorthands.hpp"
#include "Operations.hpp"
#include "MouseEvent.hpp"

namespace Lattice {
namespace Render {

class RenderableObject;
class RenderWidget;
class PickableMap;

class Viewport : public QWidget {
    Q_OBJECT
    using super = QWidget;
public:
    Viewport(QWidget* parent = nullptr);

    auto& renderables() const { return _renderables; }
    void setRenderables(const std::vector<SP<Renderable>> &renderables) { _renderables = renderables; }

    auto& camera() const { return _camera; }

    const SP<PickableMap>& pickableMap();

signals:
    void updateRequested();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void moveEvent(QMoveEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    Opt<std::pair<SP<Renderable>, double>> hitTest(glm::dvec2 pos, const SP<Camera>& camera);

    std::vector<SP<Renderable>> _renderables;
    Opt<SP<Renderable>> _draggedRenderable;
    Opt<SP<Renderable>> _hoveredRenderable;
    Opt<SP<PickableMap>> _pickableMap;
    double _hitDepth;
    SP<Camera> _camera;
};

} // namespace Renderer
} // namespace Lattice
