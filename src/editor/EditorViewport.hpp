#pragma once
#include <QOpenGLWidget>
#include "CameraController.hpp"
#include "../support/Shorthands.hpp"
#include "../viewport/Viewport.hpp"

namespace Lattice {
namespace State {
class AppState;
}

namespace Editor {

class EditorScene;
class KeyObserver;

class EditorViewport final : public Viewport::Viewport {
    Q_OBJECT
    using super = Viewport;
public:
    explicit EditorViewport(const SP<State::AppState>& appState, const SP<KeyObserver>& keyObserver, QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void wheelEvent(QWheelEvent *event) override;

private:
    CameraController _cameraController;
    SP<State::AppState> _appState;
};

}
} // namespace Lattice
