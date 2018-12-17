#include "Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace Lattice {

Camera::Camera() {
    update();
}

void Camera::setPos(glm::vec3 pos) {
    _pos = pos;
    update();
}

void Camera::setYaw(float yaw) {
    _yaw = yaw;
    update();
}

void Camera::setPitch(float pitch) {
    _pitch = pitch;
    update();
}

void Camera::update() {
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
