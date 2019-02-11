#pragma once
#include <QOpenGLWidget>
#include "../support/Shorthands.hpp"
#include "Operations.hpp"
#include "MouseEvent.hpp"

namespace Lattice {
namespace Render {

class Renderable;
class Viewport;

class ViewportContainer : public QOpenGLWidget, protected QOpenGLExtraFunctions {
    Q_OBJECT
public:
    ViewportContainer(QWidget* parent = nullptr);

    auto& viewports() const { return _viewports; }
    void setViewports(const std::vector<Viewport*>& viewports);

signals:
    void resized();
    void initialized();
    void aboutToBePainted();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:

    std::vector<Viewport*> _viewports;
    std::optional<SP<Operations>> _operations;
};

} // namespace Renderer
} // namespace Lattice
