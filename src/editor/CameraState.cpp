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
}

Camera CameraState::camera() const {
    auto cameraToWorld = glm::translate(_position) * glm::mat4_cast(glm::dquat(_eulerAngles));

    if (_projection == Camera::Projection::Perspective) {
        return Camera::perspective(cameraToWorld, _viewportSize, _fieldOfView, _zNear, _zFar);
    } else {
        return Camera::orthographic(cameraToWorld, _viewportSize, _orthoScale);
    }
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

} // namespace Editor
} // namespace Lattice
