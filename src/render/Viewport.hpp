#pragma once
#include <QOpenGLWidget>
#include "../support/Pointer.hpp"
#include "Operations.hpp"
#include "MouseEvent.hpp"

namespace Lattice {
namespace Render {

class Renderable;
class RenderWidget;

class Viewport : public QWidget, protected QOpenGLExtraFunctions {
    Q_OBJECT
    using super = QWidget;
public:
    Viewport(QWidget* parent = nullptr);

    auto& renderables() const { return _renderables; }
    void setRenderables(const std::vector<SP<Renderable>> &renderables) { _renderables = renderables; }

    void render(const SP<Operations>& operations);

    void setCameraLocation(const Location& location);
    void setCameraProjection(const Camera::Projection& projection);

    auto& camera() const { return _camera; }

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
    std::optional<std::pair<SP<Renderable>, HitResult>> hitTest(glm::dvec2 pos, const SP<Camera>& camera);

    std::vector<SP<Renderable>> _renderables;
    std::optional<SP<Renderable>> _draggedRenderable;
    HitResult _hitResult;
    SP<Camera> _camera;
};

} // namespace Renderer
} // namespace Lattice
