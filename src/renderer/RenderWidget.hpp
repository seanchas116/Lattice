#pragma once
#include <QOpenGLWidget>

namespace Lattice {
namespace Renderer {

class RenderWidget : public QOpenGLWidget {
    Q_OBJECT
public:
    RenderWidget(QWidget* parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
};

} // namespace Renderer
} // namespace Lattice
