#include "CameraController.hpp"
#include "../render/Util.hpp"
#include <QMouseEvent>
#include <QtDebug>

using namespace glm;

namespace Lattice::Editor {

CameraController::CameraController(const SP<Camera> &camera, QWidget *widget) : _camera(camera), _widget(widget) {
    Location location;
    location.position = dvec3(0, 10, 10);
    _camera->setLocation(location);
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
    Location location = _camera->location();
    switch (_mode) {
    case Mode::Move: {
        glm::dmat2x3 upRight(location.up(), location.right());
        double ratio;
        if (_camera->projection() == Camera::Projection::Perspective) {
            ratio = 0.02 / Render::widgetPixelRatio(_widget);
        } else {
            ratio = 1 / (_camera->orthoScale() * Render::widgetPixelRatio(_widget));
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
}

} // namespace Lattice
