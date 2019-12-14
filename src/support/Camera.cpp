#include "Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

namespace Lattice {

std::pair<dvec3, bool> Camera::mapModelToViewport(const dmat4 &modelMatrix, dvec3 worldPos) const {
    return mapWorldToViewport((modelMatrix * dvec4(worldPos, 1)).xyz);
}

dvec3 Camera::mapViewportToModel(const dmat4 &modelMatrix, dvec3 viewportPosWithDepth) const {
    dmat4 worldToModelMatrix = inverse(modelMatrix);
    return (worldToModelMatrix * dvec4(mapViewportToWorld(viewportPosWithDepth), 1)).xyz;
}

std::pair<glm::dvec3, bool> Camera::mapWorldToViewport(dvec3 worldPos) const {
    dvec3 pos_cameraSpace = (_worldToCameraMatrix * dvec4(worldPos, 1)).xyz;
    return mapCameraToViewport(pos_cameraSpace);
}

glm::dvec3 Camera::mapViewportToWorld(dvec3 viewportPosWithDepth) const {
    auto cameraPos = mapViewportToCamera(viewportPosWithDepth);
    return (_cameraToWorldMatrix * vec4(cameraPos, 1)).xyz;
}

std::pair<dvec3, bool> Camera::mapCameraToViewport(dvec3 cameraPos) const {
    vec4 pos_clipSpace = _cameraToViewportMatrix * vec4(cameraPos, 1);
    if (fabs(pos_clipSpace.x) <= pos_clipSpace.w && fabs(pos_clipSpace.y) <= pos_clipSpace.w && fabs(pos_clipSpace.z) <= pos_clipSpace.w) {
        vec3 ndc = vec3(pos_clipSpace.xyz) / pos_clipSpace.w;
        return {vec3((vec2(ndc.xy) + 1.f) * 0.5f * vec2(_viewportSize), ndc.z * 0.5f + 0.5f), true};
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

Camera Camera::perspective(glm::dmat4 cameraToWorldMatrix, glm::dvec2 viewportSize, double fieldOfView, double zNear, double zFar) {
    Camera camera;
    camera._projection = Projection::Perspective;
    camera._cameraToWorldMatrix = cameraToWorldMatrix;
    camera._viewportSize = viewportSize;
    camera._fieldOfView = fieldOfView;
    camera._zNear = zNear;
    camera._zFar = zFar;
    camera._orthoScale = 1;

    camera._cameraToViewportMatrix = glm::perspective(fieldOfView, viewportSize.x / viewportSize.y, zNear, zFar);
    camera._worldToCameraMatrix = inverse(cameraToWorldMatrix);
    camera._worldToViewportMatrix = camera._cameraToViewportMatrix * camera._worldToCameraMatrix;

    return camera;
}

Camera Camera::orthographic(glm::dmat4 cameraToWorldMatrix, glm::dvec2 viewportSize, double scale) {
    double zNear = -10000;
    double zFar = 10000;

    Camera camera;
    camera._projection = Projection::Orthographic;
    camera._cameraToWorldMatrix = cameraToWorldMatrix;
    camera._viewportSize = viewportSize;
    camera._fieldOfView = 0;
    camera._zNear = zNear;
    camera._zFar = zFar;
    camera._orthoScale = scale;

    dvec2 topRight = viewportSize / scale * 0.5;
    camera._cameraToViewportMatrix = ortho(-topRight.x, topRight.x, -topRight.y, topRight.y, zNear, zFar);
    camera._worldToCameraMatrix = inverse(cameraToWorldMatrix);
    camera._worldToViewportMatrix = camera._cameraToViewportMatrix * camera._worldToCameraMatrix;

    return camera;
}

} // namespace Lattice
