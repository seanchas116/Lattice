#include "CameraController.hpp"
#include <QMouseEvent>
#include <QtDebug>

using namespace glm;

namespace Lattice::Editor {

CameraController::CameraController(const SP<Camera> &camera) : _camera(camera) {
    _location.position = dvec3(0, 10, 10);
    _camera->setLocation(_location);
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
        _location.position = _location.position + _location.up() * double(offset.y()) * 0.02 + _location.right() * double(-offset.x()) * 0.02;
        _camera->setLocation(_location);
        emit cameraChanged();
        break;
    }
    case Mode::Rotate: {
        double unit = 0.2 / 180.0 * M_PI;
        _eulerAngles.y -= offset.x() * unit;
        _eulerAngles.x -= offset.y() * unit;

        _location.rotation = glm::dquat(_eulerAngles);

        _camera->setLocation(_location);
        emit cameraChanged();
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
    if (_camera->projection() == Camera::Projection::Perspective) {
        _location.position = _location.position + -_location.backward() * (0.01 * event->delta());
        _camera->setLocation(_location);
    } else {
        _camera->setOrthoScale(_camera->orthoScale() * pow(2.0, 0.001 * event->delta()));
    }
    emit cameraChanged();
    return false;
}

void CameraController::setPressedKeys(const std::unordered_set<int> &keys) {
    _pressedKeys = keys;
}

} // namespace Lattice
