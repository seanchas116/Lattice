#include "CameraController.hpp"
#include "../viewport/Util.hpp"
#include <QMouseEvent>
#include <QtDebug>
#include <QApplication>

using namespace glm;

namespace Lattice {
namespace Editor {

CameraController::CameraController(const SP<Camera> &camera, QWidget *widget) : _camera(camera), _widget(widget) {
    _camera->setLocation(location());
    connect(_camera.get(), &Camera::locationChanged, this, [this](const Location& location) {
        _position = location.position;
        _eulerAngles = glm::eulerAngles(location.rotation);
    });
}

bool CameraController::mousePress(QMouseEvent *event) {
    bool move = _moveKey || (event->button() == Qt::MiddleButton && event->modifiers() & Qt::ShiftModifier);
    bool rotate = _rotateKey || event->button() == Qt::MiddleButton;

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
    switch (_mode) {
    case Mode::Move: {
        glm::dmat2x3 upRight(location().up(), location().right());
        double ratio;
        if (_camera->projection() == Camera::Projection::Perspective) {
            ratio = 0.02;
        } else {
            ratio = 1 / _camera->orthoScale();
        }
        _position += upRight * (glm::dvec2(offset.y(), -offset.x()) * ratio);

        _camera->setLocation(location());
        break;
    }
    case Mode::Rotate: {
        double unit = 0.2 / 180.0 * M_PI;
        _eulerAngles.y -= offset.x() * unit;
        _eulerAngles.x -= offset.y() * unit;

        _camera->setLocation(location());
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
        _position += -location().backward() * (0.01 * event->delta());
        _camera->setLocation(location());
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

Location CameraController::location() const {
    Location location;
    location.position = _position;
    location.rotation = glm::dquat(_eulerAngles);
    return location;
}

}
} // namespace Lattice
