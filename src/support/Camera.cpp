#include "Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

namespace Lattice {

Camera::Camera() {
    connect(this, &Camera::projectionChanged, this, &Camera::changed);
    connect(this, &Camera::locationChanged, this, &Camera::changed);
    connect(this, &Camera::viewportSizeChanged, this, &Camera::changed);
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

std::pair<dvec3, bool> Camera::mapModelToViewport(const dmat4 &modelMatrix, dvec3 worldPos) const {
    return mapWorldToViewport((modelMatrix * dvec4(worldPos, 1)).xyz());
}

dvec3 Camera::mapViewportToModel(const dmat4 &modelMatrix, dvec3 viewportPosWithDepth) const {
    dmat4 worldToModelMatrix = inverse(modelMatrix);
    return (worldToModelMatrix * dvec4(mapViewportToWorld(viewportPosWithDepth), 1)).xyz();
}

std::pair<glm::dvec3, bool> Camera::mapWorldToViewport(dvec3 worldPos) const {
    dvec3 pos_cameraSpace = (_worldToCameraMatrix * dvec4(worldPos, 1)).xyz();
    return mapCameraToViewport(pos_cameraSpace);
}

glm::dvec3 Camera::mapViewportToWorld(dvec3 viewportPosWithDepth) const {
    auto cameraPos = mapViewportToCamera(viewportPosWithDepth);
    auto cameraToWorldMatrix = _location.matrixToWorld();
    return (cameraToWorldMatrix * vec4(cameraPos, 1)).xyz();
}

std::pair<dvec3, bool> Camera::mapCameraToViewport(dvec3 cameraPos) const {
    vec4 pos_clipSpace = _cameraToViewportMatrix * vec4(cameraPos, 1);
    if (fabs(pos_clipSpace.x) <= pos_clipSpace.w && fabs(pos_clipSpace.y) <= pos_clipSpace.w && fabs(pos_clipSpace.z) <= pos_clipSpace.w) {
        vec3 ndc = vec3(pos_clipSpace.xyz()) / pos_clipSpace.w;
        return {vec3((ndc.xy() + 1.f) * 0.5f * vec2(_viewportSize), ndc.z * 0.5f + 0.5f), true};
    }
    return {vec3(0), false};
}

dvec3 Camera::mapViewportToCamera(dvec3 viewportPosWithDepth) const {
    return glm::unProject(viewportPosWithDepth, dmat4(1), _cameraToViewportMatrix, dvec4(0, 0, _viewportSize));
}

Ray<double> Camera::cameraMouseRay(dvec2 viewportPos) const {
    dvec3 front = mapViewportToCamera(dvec3(viewportPos, -1));
    dvec3 back = mapViewportToCamera(dvec3(viewportPos, 1));
    return {front, back - front};
}

Ray<double> Camera::worldMouseRay(dvec2 viewportPos) const {
    dvec3 front = mapViewportToWorld(dvec3(viewportPos, -1));
    dvec3 back = mapViewportToWorld(dvec3(viewportPos, 1));
    return {front, back - front};
}

Ray<double> Camera::modelMouseRay(const dmat4 &modelMatrix, dvec2 viewportPos) const {
    dvec3 front = mapViewportToModel(modelMatrix, dvec3(viewportPos, -1));
    dvec3 back = mapViewportToModel(modelMatrix, dvec3(viewportPos, 1));
    return {front, back - front};
}

void Camera::updateMatrix() {
    _worldToCameraMatrix = inverse(_location.matrixToWorld());
    if (_projection == Projection::Perspective) {
        _cameraToViewportMatrix = glm::perspective(_fieldOfView, double(_viewportSize.x) / double(_viewportSize.y), _zNear, _zFar);
    } else {
        dvec2 topRight = _viewportSize / _orthoScale * 0.5;
        _cameraToViewportMatrix = glm::ortho(-topRight.x, topRight.x, -topRight.y, topRight.y, -10000.0, 10000.0);
    }
    _worldToViewportMatrix = _cameraToViewportMatrix * _worldToCameraMatrix;
}

} // namespace Lattice
