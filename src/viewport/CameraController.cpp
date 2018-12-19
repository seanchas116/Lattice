#include "CameraController.hpp"
#include <QMouseEvent>
#include <QtDebug>

namespace Lattice {

CameraController::CameraController() {
}

bool CameraController::mousePress(QMouseEvent *event) {
    bool rotateKey = _pressedKeys.find(Qt::Key_Space) != _pressedKeys.end() && event->modifiers() & Qt::ShiftModifier;
    bool moveKey = _pressedKeys.find(Qt::Key_Space) != _pressedKeys.end() && !(event->modifiers() & Qt::ShiftModifier);

    if (moveKey || event->button() == Qt::MiddleButton) {
        _mode = Mode::Move;
    } else if (rotateKey || event->button() == Qt::RightButton) {
        _mode = Mode::Rotate;
    } else {
        _mode = Mode::None;
        return false;
    }

    _lastMousePos = event->pos();
    return true;
}

bool CameraController::mouseMove(QMouseEvent *event) {
    QPoint offset = event->pos() - _lastMousePos;
    switch (_mode) {
    case Mode::Move: {
        _camera.setPos(_camera.pos() + _camera.up() * float(offset.y()) * 0.05f + -_camera.right() * float(offset.x()) * 0.05f);
        emit cameraChanged(_camera);
        break;
    }
    case Mode::Rotate: {
        float unit = 0.25f / 180.f * float(M_PI);
        _camera.setYaw(_camera.yaw() - offset.x() * unit);
        _camera.setPitch(_camera.pitch() - offset.y() * unit);
        emit cameraChanged(_camera);
        break;
    }
    default: {
        return false;
    }
    }

    _lastMousePos = event->pos();
    return true;
}

bool CameraController::mouseRelease(QMouseEvent *) {
    _mode = Mode::None;
    return false;
}

bool CameraController::wheel(QWheelEvent *event) {
    _camera.setPos(_camera.pos() + _camera.direction() * (0.01f * event->delta()));
    emit cameraChanged(_camera);
}

void CameraController::setPressedKeys(const std::unordered_set<int> &keys) {
    _pressedKeys = keys;
}

} // namespace Lattice
