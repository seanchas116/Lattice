#include "EditorViewport.hpp"
#include "KeyObserver.hpp"
#include "ViewportControlView.hpp"

namespace Lattice::Editor {

EditorViewport::EditorViewport(const SP<UI::AppState> &appState, const SP<KeyObserver> &keyObserver, QWidget *parent) :
    Render::Viewport(parent),
    _appState(appState)
{
    connect(keyObserver.get(), &KeyObserver::pressedKeysChanged, &_cameraController, &CameraController::setPressedKeys);
    setCameraLocation(_cameraController.location());
    connect(&_cameraController, &CameraController::locationChanged, this, &Render::Viewport::setCameraLocation);

    new ViewportControlView(this);
}

void EditorViewport::mousePressEvent(QMouseEvent *event) {
    if (_cameraController.mousePress(event)) {
        return;
    }
    super::mousePressEvent(event);
}

void EditorViewport::mouseMoveEvent(QMouseEvent *event) {
    if (_cameraController.mouseMove(event)) {
        return;
    }
    super::mouseMoveEvent(event);
}

void EditorViewport::mouseReleaseEvent(QMouseEvent *event) {
    if (_cameraController.mouseRelease(event)) {
        return;
    }
    super::mouseReleaseEvent(event);
}

void EditorViewport::wheelEvent(QWheelEvent *event) {
    _cameraController.wheel(event);
    super::wheelEvent(event);
}

} // namespace Lattice
