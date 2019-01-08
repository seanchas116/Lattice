#pragma once
#include <QOpenGLWidget>
#include <memory>
#include "CameraController.hpp"
#include "KeyObserver.hpp"
#include "../support/Pointer.hpp"

namespace Lattice::UI {
class AppState;
}

namespace Lattice::Editor {

class ViewportRenderer;

class ViewportWidget final : public QOpenGLWidget {
    Q_OBJECT
public:
    explicit ViewportWidget(const SP<UI::AppState>& appState, QWidget *parent = nullptr);

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
    double widgetPixelRatio() const;
    glm::dvec2 mapToRenderer(const QPoint& p);

    std::optional<SP<ViewportRenderer>> _renderer;
    CameraController _cameraController;
    KeyObserver _keyObserver;
    SP<UI::AppState> _appState;
};

} // namespace Lattice

