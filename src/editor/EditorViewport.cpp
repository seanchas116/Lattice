#include "EditorViewport.hpp"

namespace Lattice::Editor {

EditorViewport::EditorViewport(const SP<UI::AppState> &appState, QWidget *parent) :
    Render::Viewport(parent),
    _appState(appState)
{
    setCameraLocation(_cameraController.location());
    connect(&_cameraController, &CameraController::locationChanged, this, &Render::Viewport::setCameraLocation);
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
