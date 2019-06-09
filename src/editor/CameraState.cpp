#include "CameraState.hpp"
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace Lattice {
namespace Editor {

CameraState::CameraState() {
    connect(this, &CameraState::orientationChanged, this, [this] (auto orientation) {
        if (orientation == Orientation::None) {
            return;
        }
        setEulerAngles(orientationAngle(orientation));
    });
    connect(this, &CameraState::eulerAnglesChanged, this, [this] {
        setOrientation(Orientation::None);
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