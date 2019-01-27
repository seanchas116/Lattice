#include "EditorViewport.hpp"
#include "KeyObserver.hpp"
#include "ViewportControlView.hpp"
#include <QVBoxLayout>

namespace Lattice::Editor {

EditorViewport::EditorViewport(const SP<UI::AppState> &appState, const SP<KeyObserver> &keyObserver, QWidget *parent) :
    Render::Viewport(parent),
    _cameraController(camera()),
    _appState(appState)
{
    connect(keyObserver.get(), &KeyObserver::pressedKeysChanged, &_cameraController, &CameraController::setPressedKeys);
    connect(&_cameraController, &CameraController::cameraChanged, this, &Render::Viewport::updateRequested);

    auto layout = new QVBoxLayout();
    layout->setAlignment(Qt::AlignTop | Qt::AlignRight);

    auto controlView = new ViewportControlView(this);
    connect(controlView, &ViewportControlView::cameraProjectionChanged, this, &Render::Viewport::setCameraProjection);
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

} // namespace Lattice
