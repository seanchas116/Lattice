#pragma once
#include <QOpenGLWidget>
#include "CameraController.hpp"
#include "../support/Shorthands.hpp"
#include "../render/Viewport.hpp"

namespace Lattice::UI {
class AppState;
}

namespace Lattice::Editor {

class EditorScene;
class KeyObserver;

class EditorViewport final : public Render::Viewport {
    Q_OBJECT
    using super = Render::Viewport;
public:
    explicit EditorViewport(const SP<UI::AppState>& appState, const SP<KeyObserver>& keyObserver, QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void wheelEvent(QWheelEvent *event) override;

private:
    CameraController _cameraController;
    SP<UI::AppState> _appState;
};

} // namespace Lattice
