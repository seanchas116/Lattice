#include "CameraState.hpp"
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

namespace Lattice {
namespace Editor {

CameraState::CameraState() {
    connect(this, &CameraState::eulerAnglesChanged, this, [this] {
        emit orientationChanged(orientation());
    });

    connect(this, &CameraState::orientationChanged, this, &CameraState::emitCameraChanged);
    connect(this, &CameraState::projectionChanged, this, &CameraState::emitCameraChanged);
    connect(this, &CameraState::positionChanged, this, &CameraState::emitCameraChanged);
    connect(this, &CameraState::eulerAnglesChanged, this, &CameraState::emitCameraChanged);
    connect(this, &CameraState::viewportSizeChanged, this, &CameraState::emitCameraChanged);
    connect(this, &CameraState::fieldOfViewChanged, this, &CameraState::emitCameraChanged);
    connect(this, &CameraState::zNearChanged, this, &CameraState::emitCameraChanged);
    connect(this, &CameraState::zFarChanged, this, &CameraState::emitCameraChanged);
    connect(this, &CameraState::orthoScaleChanged, this, &CameraState::emitCameraChanged);
}

Camera CameraState::camera() const {
    if (_projection == Camera::Projection::Perspective) {
        return Camera::perspective(cameraToWorldMatrix(), _viewportSize, _fieldOfView, _zNear, _zFar);
    } else {
        return Camera::orthographic(cameraToWorldMatrix(), _viewportSize, _orthoScale);
    }
}

glm::dmat4 CameraState::cameraToWorldMatrix() const {
    return glm::translate(_position) * glm::mat4_cast(glm::dquat(_eulerAngles));
}

CameraState::Orientation CameraState::orientation() const {
    Orientation orientations[] = {
        Orientation::Front,
        Orientation::Back,
        Orientation::Right,
        Orientation::Left,
        Orientation::Top,
        Orientation::Bottom,
    };

    for (auto orientation : orientations) {
        if (_eulerAngles == orientationAngle(orientation)) {
            return orientation;
        }
    }
    return Orientation::None;
}

void CameraState::setOrientation(CameraState::Orientation orientation) {
    setEulerAngles(orientationAngle(orientation));
}

glm::dvec3 CameraState::orientationAngle(CameraState::Orientation orientation) {
    switch (orientation) {
    case Orientation::None:
    case Orientation::Front:
        return {0, 0, 0};
    case Orientation::Back:
        return {0, M_PI, 0};
    case Orientation::Right:
        return {0, M_PI * 0.5, 0};
    case Orientation::Left:
        return {0, M_PI * 1.5, 0};
    case Orientation::Top:
        return {M_PI * -0.5, 0, 0};
    case Orientation::Bottom:
        return {M_PI * 0.5, 0, 0};
    }
}

void CameraState::emitCameraChanged() {
    emit cameraChanged(camera());
}

} // namespace Editor
} // namespace Lattice
