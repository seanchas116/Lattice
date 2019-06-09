#include "CameraState.hpp"

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
