#include "OldCamera.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace Lattice {

OldCamera::OldCamera() {
    update();
}

void OldCamera::setPos(glm::vec3 pos) {
    _pos = pos;
    update();
}

void OldCamera::setYaw(float yaw) {
    _yaw = yaw;
    update();
}

void OldCamera::setPitch(float pitch) {
    _pitch = pitch;
    update();
}

void OldCamera::update() {
    _direction = glm::vec3(
                cos(_pitch) * sin(_yaw),
        sin(_pitch),
        cos(_pitch) * cos(_yaw)
    );
    _right = glm::vec3(
        sin(_yaw - float(M_PI) * 0.5f),
        0,
        cos(_yaw - float(M_PI) * 0.5f)
    );
    _up = glm::cross(_right, _direction);
    _matrix = glm::lookAt(_pos, _pos + _direction, _up);
}

} // namespace Lattice
