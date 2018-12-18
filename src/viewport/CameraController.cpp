#include "CameraController.hpp"
#include <QMouseEvent>

namespace Lattice {

CameraController::CameraController() {
}

bool CameraController::mousePress(QMouseEvent *event) {
    if (event->button() == Qt::MiddleButton) {
        _mode = Mode::Move;
    } else if (event->button() == Qt::RightButton) {
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

} // namespace Lattice
