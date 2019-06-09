#include "OldCamera.hpp"
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

namespace Lattice {

dvec3 OldCamera::orientationAngle(Orientation orientation) {
    switch (orientation) {
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

OldCamera::OldCamera() {
    connect(this, &OldCamera::projectionChanged, this, &OldCamera::changed);
    connect(this, &OldCamera::locationChanged, this, &OldCamera::changed);
    connect(this, &OldCamera::viewportSizeChanged, this, &OldCamera::changed);
    connect(this, &OldCamera::fieldOfViewChanged, this, &OldCamera::changed);
    connect(this, &OldCamera::zNearChanged, this, &OldCamera::changed);
    connect(this, &OldCamera::zFarChanged, this, &OldCamera::changed);
    connect(this, &OldCamera::orthoScaleChanged, this, &OldCamera::changed);
    connect(this, &OldCamera::changed, this, &OldCamera::updateMatrix);
}

void OldCamera::lookOrientation(OldCamera::Orientation orientation) {
    auto location = this->location();
    location.rotation = glm::dquat(orientationAngle(orientation));
    setLocation(location);
}

bool OldCamera::isLookingOrientation(OldCamera::Orientation orientation) const {
    return _location.rotation == glm::dquat(orientationAngle(orientation));
}

std::pair<dvec3, bool> OldCamera::mapModelToViewport(const dmat4 &modelMatrix, dvec3 worldPos) const {
    return mapWorldToViewport((modelMatrix * dvec4(worldPos, 1)).xyz);
}

dvec3 OldCamera::mapViewportToModel(const dmat4 &modelMatrix, dvec3 viewportPosWithDepth) const {
    dmat4 worldToModelMatrix = inverse(modelMatrix);
    return (worldToModelMatrix * dvec4(mapViewportToWorld(viewportPosWithDepth), 1)).xyz;
}

std::pair<glm::dvec3, bool> OldCamera::mapWorldToViewport(dvec3 worldPos) const {
    dvec3 pos_cameraSpace = (_worldToCameraMatrix * dvec4(worldPos, 1)).xyz;
    return mapCameraToViewport(pos_cameraSpace);
}

glm::dvec3 OldCamera::mapViewportToWorld(dvec3 viewportPosWithDepth) const {
    auto cameraPos = mapViewportToCamera(viewportPosWithDepth);
    auto cameraToWorldMatrix = _location.matrixToWorld();
    return (cameraToWorldMatrix * vec4(cameraPos, 1)).xyz;
}

std::pair<dvec3, bool> OldCamera::mapCameraToViewport(dvec3 cameraPos) const {
    vec4 pos_clipSpace = _cameraToViewportMatrix * vec4(cameraPos, 1);
    if (fabs(pos_clipSpace.x) <= pos_clipSpace.w && fabs(pos_clipSpace.y) <= pos_clipSpace.w && fabs(pos_clipSpace.z) <= pos_clipSpace.w) {
        vec3 ndc = vec3(pos_clipSpace.xyz) / pos_clipSpace.w;
        return {vec3((vec2(ndc.xy) + 1.f) * 0.5f * vec2(_viewportSize), ndc.z * 0.5f + 0.5f), true};
    }
    return {vec3(0), false};
}

dvec3 OldCamera::mapViewportToCamera(dvec3 viewportPosWithDepth) const {
    return glm::unProject(viewportPosWithDepth, dmat4(1), _cameraToViewportMatrix, dvec4(0, 0, _viewportSize));
}

Ray<double> OldCamera::cameraMouseRay(dvec2 viewportPos) const {
    dvec3 front = mapViewportToCamera(dvec3(viewportPos, -1));
    dvec3 back = mapViewportToCamera(dvec3(viewportPos, 1));
    return {front, back - front};
}

Ray<double> OldCamera::worldMouseRay(dvec2 viewportPos) const {
    dvec3 front = mapViewportToWorld(dvec3(viewportPos, -1));
    dvec3 back = mapViewportToWorld(dvec3(viewportPos, 1));
    return {front, back - front};
}

Ray<double> OldCamera::modelMouseRay(const dmat4 &modelMatrix, dvec2 viewportPos) const {
    dvec3 front = mapViewportToModel(modelMatrix, dvec3(viewportPos, -1));
    dvec3 back = mapViewportToModel(modelMatrix, dvec3(viewportPos, 1));
    return {front, back - front};
}

void OldCamera::updateMatrix() {
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
