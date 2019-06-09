#include "EditorViewport.hpp"
#include "KeyObserver.hpp"
#include "ViewportControlView.hpp"
#include "CameraState.hpp"
#include <QVBoxLayout>

namespace Lattice {
namespace Editor {

EditorViewport::EditorViewport(const SP<State::AppState> &appState, const SP<KeyObserver> &keyObserver, QWidget *parent) :
    Viewport::Viewport(parent),
    _appState(appState),
    _cameraState(makeShared<CameraState>()),
    _cameraController(_cameraState, this)
{
    connect(keyObserver.get(), &KeyObserver::pressedKeysChanged, &_cameraController, &CameraController::setPressedKeys);
    connect(_cameraState.get(), &CameraState::cameraChanged, this, &Viewport::setCamera);
    setCamera(_cameraState->camera());

    auto layout = new QVBoxLayout();
    layout->setAlignment(Qt::AlignTop | Qt::AlignRight);

    auto controlView = new ViewportControlView(_cameraState, this);
    layout->addWidget(controlView);

    setLayout(layout);
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

void EditorViewport::resizeEvent(QResizeEvent *event) {
    super::resizeEvent(event);
    _cameraState->setViewportSize(glm::vec2(event->size().width(), event->size().height()));
}

}
} // namespace Lattice
