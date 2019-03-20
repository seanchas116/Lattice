#include "Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

namespace Lattice {

Camera::Camera() {
    connect(this, &Camera::projectionChanged, this, &Camera::changed);
    connect(this, &Camera::locationChanged, this, &Camera::changed);
    connect(this, &Camera::viewSizeChanged, this, &Camera::changed);
    connect(this, &Camera::fieldOfViewChanged, this, &Camera::changed);
    connect(this, &Camera::zNearChanged, this, &Camera::changed);
    connect(this, &Camera::zFarChanged, this, &Camera::changed);
    connect(this, &Camera::orthoScaleChanged, this, &Camera::changed);
    connect(this, &Camera::changed, this, &Camera::updateMatrix);
}

void Camera::lookFront() {
    auto location = this->location();
    location.rotation = glm::dquat(glm::vec3(0, 0, 0));
    setLocation(location);
}

std::pair<dvec3, bool> Camera::mapModelToScreen(const dmat4 &modelMatrix, dvec3 worldPos) const {
    return mapWorldToScreen((modelMatrix * dvec4(worldPos, 1)).xyz);
}

dvec3 Camera::mapScreenToModel(const dmat4 &modelMatrix, dvec3 screenPosWithDepth) const {
    dmat4 worldToModelMatrix = inverse(modelMatrix);
    return (worldToModelMatrix * dvec4(mapScreenToWorld(screenPosWithDepth), 1)).xyz;
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

Ray<double> Camera::modelMouseRay(const dmat4 &modelMatrix, dvec2 screenPos) const {
    dvec3 front = mapScreenToModel(modelMatrix, dvec3(screenPos, -1));
    dvec3 back = mapScreenToModel(modelMatrix, dvec3(screenPos, 1));
    return {front, back - front};
}

void Camera::updateMatrix() {
    _worldToCameraMatrix = inverse(_location.matrixToWorld());
    if (_projection == Projection::Perspective) {
        _cameraToScreenMatrix = glm::perspective(_fieldOfView, double(_viewSize.x) / double(_viewSize.y), _zNear, _zFar);
    } else {
        dvec2 topRight = _viewSize / _orthoScale * 0.5;
        _cameraToScreenMatrix = glm::ortho(-topRight.x, topRight.x, -topRight.y, topRight.y, -10000.0, 10000.0);
    }
    _worldToScreenMatrix = _cameraToScreenMatrix * _worldToCameraMatrix;
}

} // namespace Lattice
