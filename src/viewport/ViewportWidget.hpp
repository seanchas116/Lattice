#pragma once
#include <QOpenGLWidget>
#include <memory>
#include "../support/Pointer.hpp"

namespace Lattice {

class ViewportRenderer;
class CameraController;

class ViewportWidget : public QOpenGLWidget {
    Q_OBJECT
public:
    explicit ViewportWidget(QWidget *parent = nullptr);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    SP<ViewportRenderer> _renderer;
    SP<CameraController> _cameraController;
};

} // namespace Lattice

