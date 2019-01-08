#pragma once
#include <QOpenGLWidget>
#include "../support/Pointer.hpp"
#include "Viewport.hpp"
#include "Operations.hpp"

namespace Lattice {
namespace Render {

class Renderable;

using Layer = std::vector<SP<Renderable>>;

class RenderWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions {
    Q_OBJECT
public:
    RenderWidget(QWidget* parent = nullptr);

    auto& viewports() const { return _viewports; }
    void setViewports(const std::vector<Viewport>& viewports) { _viewports = viewports; }

    std::vector<Layer> layers() const { return _layers; }
    void setLayers(const std::vector<Layer> &layers) { _layers = layers; }

    glm::ivec2 logicalSize() const { return _logicalSize; }

signals:
    void sizeChanged(glm::ivec2 logicalSize, glm::ivec2 physicalSize);

protected:
    void resizeEvent(QResizeEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    double widgetPixelRatio() const;

    glm::ivec2 _logicalSize;
    Operations _operations;
    std::vector<Layer> _layers;
    std::vector<Viewport> _viewports;
};

} // namespace Renderer
} // namespace Lattice
