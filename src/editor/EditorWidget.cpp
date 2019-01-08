#include "EditorWidget.hpp"
#include "ViewportRenderer.hpp"
#include "CameraController.hpp"
#include <QOpenGLDebugLogger>
#include <QMouseEvent>

namespace Lattice::Editor {

EditorWidget::EditorWidget(const SP<UI::AppState> &appState, QWidget *parent) :
    Render::RenderWidget(parent),
    _appState(appState)
{
    setFocusPolicy(Qt::ClickFocus);
    connect(&_keyObserver, &KeyObserver::selectedKeysChanged, &_cameraController, &CameraController::setPressedKeys);
}

void EditorWidget::mousePressEvent(QMouseEvent *event) {
    if (_cameraController.mousePress(event)) {
        return;
    }
    RenderWidget::mousePressEvent(event);
}

void EditorWidget::mouseMoveEvent(QMouseEvent *event) {
    if (_cameraController.mouseMove(event)) {
        return;
    }
    RenderWidget::mouseMoveEvent(event);
}

void EditorWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (_cameraController.mouseRelease(event)) {
        return;
    }
    RenderWidget::mouseReleaseEvent(event);
}

void EditorWidget::wheelEvent(QWheelEvent *event) {
    _cameraController.wheel(event);
    RenderWidget::wheelEvent(event);
}

void EditorWidget::keyPressEvent(QKeyEvent *event) {
    _keyObserver.keyPress(event);
    RenderWidget::keyPressEvent(event);
}

void EditorWidget::keyReleaseEvent(QKeyEvent *event) {
    _keyObserver.keyRelease(event);
    RenderWidget::keyReleaseEvent(event);
}

void EditorWidget::updateCamera() {
    _camera.setLocation(_cameraController.location());
    _camera.setViewSize(logicalSize());

    setViewports({{glm::ivec2(0), _camera}});
}

} // namespace Lattice
