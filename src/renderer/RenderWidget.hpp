#pragma once
#include <QOpenGLWidget>
#include "../support/Pointer.hpp"
#include "Viewport.hpp"
#include "Operations.hpp"

namespace Lattice {
namespace Renderer {

class Renderable;

class RenderWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions {
    Q_OBJECT
public:
    RenderWidget(QWidget* parent = nullptr);

    auto& viewports() const { return _viewports; }
    void setViewports(const std::vector<Viewport>& viewports) { _viewports = viewports; }

    auto& renderables() const { return _renderables; }
    void setRenderables(const std::vector<SP<Renderable>>& renderables) { _renderables = renderables; }

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    double widgetPixelRatio() const;

    Operations _operations;
    std::vector<SP<Renderable>> _renderables;
    std::vector<Viewport> _viewports;
};

} // namespace Renderer
} // namespace Lattice
