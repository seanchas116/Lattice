#pragma once
#include "../draw/Operations.hpp"
#include "../support/Shorthands.hpp"
#include <QOpenGLWidget>

namespace Lattice {
namespace Viewport {

class Viewport;

class ViewportContainer : public QOpenGLWidget, protected QOpenGLExtraFunctions {
    Q_OBJECT
  public:
    ViewportContainer(QWidget *parent = nullptr);

    auto &viewports() const { return _viewports; }
    void setViewports(const std::vector<Viewport *> &viewports);

  signals:
    void resized();
    void initialized();
    void aboutToBePainted();

  protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

  private:
    std::vector<Viewport *> _viewports;
    Opt<SP<Draw::Operations>> _operations;
};

} // namespace Viewport
} // namespace Lattice
