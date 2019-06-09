#include "Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace Lattice {

Camera Camera::perspective(glm::mat4 viewportToWorldMatrix, glm::vec2 viewportSize, float fieldOfView, float zNear, float zFar) {
    Camera camera;
    camera._projection = Projection::Perspective;
    camera._viewportToWorldMatrix = viewportToWorldMatrix;
    camera._viewportSize = viewportSize;
    camera._fieldOfView = fieldOfView;
    camera._zNear = zNear;
    camera._zFar = zFar;
    camera._orthoScale = 1;

    camera._cameraToViewportMatrix = glm::perspective(fieldOfView, viewportSize.x / viewportSize.y, zNear, zFar);
    camera._worldToCameraMatrix = inverse(viewportToWorldMatrix);
    camera._worldToViewportMatrix = camera._cameraToViewportMatrix * camera._worldToCameraMatrix;

    return camera;
}

Camera Camera::orthographic(glm::mat4 viewportToWorldMatrix, glm::vec2 viewportSize, float scale) {
    float zNear = -10000;
    float zFar = 10000;

    Camera camera;
    camera._projection = Projection::Orthographic;
    camera._viewportToWorldMatrix = viewportToWorldMatrix;
    camera._viewportSize = viewportSize;
    camera._fieldOfView = 0;
    camera._zNear = zNear;
    camera._zFar = zFar;
    camera._orthoScale = scale;

    glm::vec2 topRight = viewportSize / scale * 0.5f;
    camera._cameraToViewportMatrix = glm::ortho(-topRight.x, topRight.x, -topRight.y, topRight.y, zNear, zFar);
    camera._worldToCameraMatrix = inverse(viewportToWorldMatrix);
    camera._worldToViewportMatrix = camera._cameraToViewportMatrix * camera._worldToCameraMatrix;

    return camera;
}

} // namespace Lattice
