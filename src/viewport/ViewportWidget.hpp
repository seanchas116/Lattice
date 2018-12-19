#pragma once
#include <QOpenGLWidget>
#include <memory>
#include "CameraController.hpp"
#include "KeyObserver.hpp"
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

    void wheelEvent(QWheelEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    SP<ViewportRenderer> _renderer;
    CameraController _cameraController;
    KeyObserver _keyObserver;
};

} // namespace Lattice

