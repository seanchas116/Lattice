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

void Camera::setProjection(Camera::Projection projection) {
    _projection = projection;
    updateMatrix();
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

void Camera::setOrthoScale(double orthoScale) {
    _orthoScale = orthoScale;
    updateMatrix();
}

std::pair<glm::dvec3, bool> Camera::mapWorldToScreen(dvec3 worldPos) const {
    dvec3 pos_cameraSpace = (_worldToCameraMatrix * dvec4(worldPos, 1)).xyz;
    return mapCameraToScreen(pos_cameraSpace);
}

glm::dvec3 Camera::mapScreenToWorld(dvec3 screenPosWithDepth) const {
    auto cameraPos = mapScreenToCamera(screenPosWithDepth);
    auto cameraToWorldMatrix = _location.matrixToWorld();
    return (cameraToWorldMatrix * vec4(cameraPos, 1)).xyz;
}

std::pair<dvec3, bool> Camera::mapCameraToScreen(dvec3 cameraPos) const {
    vec4 pos_clipSpace = _cameraToScreenMatrix * vec4(cameraPos, 1);
    if (fabs(pos_clipSpace.x) <= pos_clipSpace.w && fabs(pos_clipSpace.y) <= pos_clipSpace.w && fabs(pos_clipSpace.z) <= pos_clipSpace.w) {
        vec3 ndc = vec3(pos_clipSpace.xyz) / pos_clipSpace.w;
        return {vec3((vec2(ndc.xy) + 1.f) * 0.5f * vec2(_viewSize), ndc.z * 0.5f + 0.5f), true};
    }
    return {vec3(0), false};
}

dvec3 Camera::mapScreenToCamera(dvec3 screenPosWithDepth) const {
    return glm::unProject(screenPosWithDepth, dmat4(1), _cameraToScreenMatrix, dvec4(0, 0, _viewSize));
}

Ray<double> Camera::cameraMouseRay(dvec2 screenPos) const {
    dvec3 front = mapScreenToCamera(dvec3(screenPos, -1));
    dvec3 back = mapScreenToCamera(dvec3(screenPos, 1));
    return {front, back - front};
}

Ray<double> Camera::worldMouseRay(dvec2 screenPos) const {
    dvec3 front = mapScreenToWorld(dvec3(screenPos, -1));
    dvec3 back = mapScreenToWorld(dvec3(screenPos, 1));
    return {front, back - front};
}

void Camera::updateMatrix() {
    _worldToCameraMatrix = inverse(_location.matrixToWorld());
    if (_projection == Projection::Perspective) {
        _cameraToScreenMatrix = glm::perspective(_fieldOfView, double(_viewSize.x) / double(_viewSize.y), _zNear, _zFar);
    } else {
        dvec2 topRight = _viewSize / _orthoScale * 0.5;
        _cameraToScreenMatrix = glm::ortho(-topRight.x, topRight.x, -topRight.y, topRight.y, _zNear, _zFar);
    }
    _worldToScreenMatrix = _cameraToScreenMatrix * _worldToCameraMatrix;
}

} // namespace Lattice
