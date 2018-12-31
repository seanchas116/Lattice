#include "CameraController.hpp"
#include <QMouseEvent>
#include <QtDebug>

using namespace glm;

namespace Lattice {

CameraController::CameraController() {
    _location.position = vec3(0, 10, 10);
    _location.rotation.y = float(-M_PI);
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
        _location.position = _location.position + _location.up() * float(offset.y()) * 0.05f + _location.right() * float(-offset.x()) * 0.05f;
        emit locationChanged(_location);
        break;
    }
    case Mode::Rotate: {
        float unit = 0.25f / 180.f * float(M_PI);
        _location.rotation.y -= -offset.x() * unit;
        _location.rotation.x -= offset.y() * unit;
        emit locationChanged(_location);
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
    _location.position = _location.position + -_location.backward() * (0.01f * event->delta());
    emit locationChanged(_location);
    return false;
}

void CameraController::setPressedKeys(const std::unordered_set<int> &keys) {
    _pressedKeys = keys;
}

} // namespace Lattice
