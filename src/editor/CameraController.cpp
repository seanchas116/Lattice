#include "CameraController.hpp"
#include "../render/Util.hpp"
#include <QMouseEvent>
#include <QtDebug>
#include <QApplication>

using namespace glm;

namespace Lattice::Editor {

CameraController::CameraController(const SP<Camera> &camera, QWidget *widget) : _camera(camera), _widget(widget) {
    Location location;
    location.position = dvec3(0, 10, 10);
    _camera->setLocation(location);
}

bool CameraController::mousePress(QMouseEvent *event) {
    bool move = _moveKey || event->button() == Qt::MiddleButton;
    bool rotate = _rotateKey || event->button() == Qt::RightButton;

    if (move) {
        _mode = Mode::Move;
    } else if (rotate) {
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
    Location location = _camera->location();
    switch (_mode) {
    case Mode::Move: {
        glm::dmat2x3 upRight(location.up(), location.right());
        double ratio;
        if (_camera->projection() == Camera::Projection::Perspective) {
            ratio = 0.02;
        } else {
            ratio = 1 / _camera->orthoScale();
        }
        location.position += upRight * (glm::dvec2(offset.y(), -offset.x()) * ratio);
        _camera->setLocation(location);
        break;
    }
    case Mode::Rotate: {
        double unit = 0.2 / 180.0 * M_PI;
        dvec3 eulerAngles = glm::eulerAngles(location.rotation);
        eulerAngles.y -= offset.x() * unit;
        eulerAngles.x -= offset.y() * unit;
        location.rotation = glm::dquat(eulerAngles);

        _camera->setLocation(location);
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
        auto location = _camera->location();
        location.position += -location.backward() * (0.01 * event->delta());
        _camera->setLocation(location);
    } else {
        _camera->setOrthoScale(_camera->orthoScale() * pow(2.0, 0.001 * event->delta()));
    }
    return false;
}

void CameraController::setPressedKeys(const std::unordered_set<int> &keys) {
    _pressedKeys = keys;

    _rotateKey = _pressedKeys.find(Qt::Key_Space) != _pressedKeys.end() && _pressedKeys.find(Qt::Key_Shift) != _pressedKeys.end();
    _moveKey = _pressedKeys.find(Qt::Key_Space) != _pressedKeys.end() && _pressedKeys.find(Qt::Key_Shift) == _pressedKeys.end();

    if (_rotateKey || _moveKey) {
        if (!_isOverridingCursor) {
            QApplication::setOverrideCursor(Qt::OpenHandCursor);
            _isOverridingCursor = true;
        }
    } else if (_isOverridingCursor) {
        QApplication::restoreOverrideCursor();
        _isOverridingCursor = false;
    }
}

} // namespace Lattice
