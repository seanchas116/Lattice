#include "Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

namespace Lattice {

Camera::Camera() :
    _viewSize(100, 100),
    _fieldOfView(glm::radians(60.0)),
    _zNear(0.1),
    _zFar(100.0)
{
}

void Camera::setLocation(const Location &location) {
    _location = location;
    updateMatrix();
}

void Camera::setViewSize(const dvec2 &viewSize) {
    _viewSize = viewSize;
    updateMatrix();
}

void Camera::setFieldOfView(double fieldOfView) {
    _fieldOfView = fieldOfView;
    updateMatrix();
}

void Camera::setZNear(double zNear) {
    _zNear = zNear;
    updateMatrix();
}

void Camera::setZFar(double zFar) {
    _zFar = zFar;
    updateMatrix();
}

std::pair<glm::dvec3, bool> Camera::mapWorldToScreen(dvec3 worldPos) const {
    dvec3 pos_cameraSpace = (_worldToCameraMatrix * dvec4(worldPos, 1)).xyz;
    return mapCameraToScreen(pos_cameraSpace);
}

glm::dvec3 Camera::mapScreenToWorld(dvec3 screenPos) const {
    auto cameraPos = mapScreenToCamera(screenPos);
    auto cameraToWorldMatrix = _location.matrix();
    return (cameraToWorldMatrix * vec4(cameraPos, 1)).xyz;
}

std::pair<dvec3, bool> Camera::mapCameraToScreen(dvec3 cameraPos) const {
    vec4 pos_clipSpace = _cameraToScreenMatrix * vec4(cameraPos, 1);
    if (fabs(pos_clipSpace.x) <= pos_clipSpace.w && fabs(pos_clipSpace.y) <= pos_clipSpace.w && fabs(pos_clipSpace.z) <= pos_clipSpace.w) {
        vec3 ndc = vec3(pos_clipSpace.xyz) / pos_clipSpace.w;
        return {vec3((vec2(ndc.xy) + 1.f) * 0.5f * vec2(_viewSize), ndc.z), true};
    }
    return {vec3(0), false};
}

dvec3 Camera::mapScreenToCamera(dvec3 screenPos) const {
    return glm::unProject(screenPos, dmat4(1), _cameraToScreenMatrix, dvec4(0, 0, _viewSize));
}

void Camera::updateMatrix() {
    _worldToCameraMatrix = inverse(_location.matrix());
    _cameraToScreenMatrix = glm::perspective(_fieldOfView, double(_viewSize.x) / double(_viewSize.y), _zNear, _zFar);
    _worldToScreenMatrix = _cameraToScreenMatrix * _worldToCameraMatrix;
}

} // namespace Lattice
