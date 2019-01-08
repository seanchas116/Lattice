#pragma once
#include <QOpenGLWidget>
#include <memory>
#include "CameraController.hpp"
#include "KeyObserver.hpp"
#include "../support/Pointer.hpp"
#include "../render/RenderWidget.hpp"

namespace Lattice::UI {
class AppState;
}

namespace Lattice::Editor {

class EditorWidget final : public Render::RenderWidget {
    Q_OBJECT
public:
    explicit EditorWidget(const SP<UI::AppState>& appState, QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void wheelEvent(QWheelEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    void updateCamera();

    CameraController _cameraController;
    KeyObserver _keyObserver;
    Camera _camera;
    SP<UI::AppState> _appState;
};

} // namespace Lattice
