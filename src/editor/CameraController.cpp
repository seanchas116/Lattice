#include "CameraController.hpp"
#include "CameraState.hpp"
#include "../viewport/Util.hpp"
#include <QMouseEvent>
#include <QtDebug>
#include <QApplication>
#include <glm/gtx/euler_angles.hpp>

using namespace glm;

namespace Lattice {
namespace Editor {

CameraController::CameraController(const SP<CameraState> &cameraState, QWidget *widget) : _cameraState(cameraState), _widget(widget) {
    /*
    _camera->setLocation(location());
    connect(_camera.get(), &OldCamera::locationChanged, this, [this](const Location& location) {
        _position = location.position;
        _eulerAngles = glm::eulerAngles(location.rotation);
    });
    */
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
        auto cameraToWorld = _cameraState->cameraToWorldMatrix();
        glm::dmat2x3 upRight(cameraToWorld[1].xyz, cameraToWorld[0].xyz);
        double ratio;
        if (_cameraState->projection() == Camera::Projection::Perspective) {
            ratio = 0.02;
        } else {
            ratio = 1 / _cameraState->orthoScale();
        }
        auto position = _cameraState->position();
        position += upRight * (glm::dvec2(offset.y(), -offset.x()) * ratio);
        _cameraState->setPosition(position);
        break;
    }
    case Mode::Rotate: {
        double unit = 0.2 / 180.0 * M_PI;
        auto eulerAngles = _cameraState->eulerAngles();
        eulerAngles.y -= offset.x() * unit;
        eulerAngles.x -= offset.y() * unit;
        _cameraState->setEulerAngles(eulerAngles);
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
    if (_cameraState->projection() == Camera::Projection::Perspective) {
        auto position = _cameraState->position();
        position += -glm::dvec3(_cameraState->cameraToWorldMatrix()[2].xyz) * (0.01 * event->delta());
        _cameraState->setPosition(position);
    } else {
        _cameraState->setOrthoScale(_cameraState->orthoScale() * pow(2.0, 0.001 * event->delta()));
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

}
} // namespace Lattice
