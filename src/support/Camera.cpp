#include "Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

namespace Lattice {

Camera::Camera() :
    _viewSize(100, 100),
    _fieldOfView(glm::radians(60.f)),
    _zNear(0.1f),
    _zFar(100.f)
{
}

void Camera::setLocation(const Location &location) {
    _location = location;
    updateMatrix();
}

void Camera::setViewSize(const glm::vec2 &viewSize) {
    _viewSize = viewSize;
    updateMatrix();
}

void Camera::setFieldOfView(float fieldOfView) {
    _fieldOfView = fieldOfView;
    updateMatrix();
}

void Camera::setZNear(float zNear) {
    _zNear = zNear;
    updateMatrix();
}

void Camera::setZFar(float zFar) {
    _zFar = zFar;
    updateMatrix();
}

std::pair<glm::vec3, bool> Camera::project(const glm::vec3 &worldPos) const {
    vec4 pos_clipSpace = _viewProjectionMatrix * vec4(worldPos, 1);
    if (fabs(pos_clipSpace.x) <= pos_clipSpace.w && fabs(pos_clipSpace.y) <= pos_clipSpace.w && fabs(pos_clipSpace.z) <= pos_clipSpace.w) {
        vec3 ndc = vec3(pos_clipSpace.xyz) / pos_clipSpace.w;
        return {vec3((vec2(ndc.xy) + 1.f) * 0.5f * vec2(_viewSize), ndc.z), true};
    }
    return {vec3(0), false};
}

glm::vec3 Camera::unProject(const glm::vec3 &screenPos) const {
    auto inverseViewMatrix = inverse(_viewMatrix);
    auto cameraPos = glm::unProject(screenPos, mat4(1), _projectionMatrix, vec4(0, 0, _viewSize));
    return (inverseViewMatrix * vec4(cameraPos, 1)).xyz;
}

void Camera::updateMatrix() {
    _viewMatrix = _location.matrix();
    _projectionMatrix = glm::perspective(_fieldOfView, float(_viewSize.x) / float(_viewSize.y), _zNear, _zFar);
    _viewProjectionMatrix = _projectionMatrix * _viewMatrix;
}

} // namespace Lattice
