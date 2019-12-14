#pragma once
#include "../support/Shorthands.hpp"
#include "../viewport/Viewport.hpp"
#include "CameraController.hpp"
#include <QOpenGLWidget>

namespace Lattice {
namespace State {
class AppState;
}

namespace Editor {

class EditorScene;
class KeyObserver;
class CameraState;

class EditorViewport final : public Viewport::Viewport {
    Q_OBJECT
    using super = Viewport;

  public:
    explicit EditorViewport(const SP<State::AppState> &appState, const SP<KeyObserver> &keyObserver, QWidget *parent = nullptr);

    auto &cameraState() const { return _cameraState; }

  protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void wheelEvent(QWheelEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

  private:
    SP<State::AppState> _appState;
    SP<CameraState> _cameraState;
    CameraController _cameraController;
};

} // namespace Editor
} // namespace Lattice
