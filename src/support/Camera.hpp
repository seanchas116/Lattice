#pragma once
#include <glm/glm.hpp>

namespace Lattice {

class Camera {
public:
    enum class Projection {
        Perspective,
        Orthographic,
    };

    static Camera perspective(glm::mat4 viewportToWorldMatrix, glm::vec2 viewportSize, float fieldOfView, float zNear, float zFar);
    static Camera orthographic(glm::mat4 viewportToWorldMatrix, glm::vec2 viewportSize, float scale);

private:
    Camera() {}

    Projection _projection;
    glm::mat4 _viewportToWorldMatrix;
    glm::vec2 _viewportSize;
    float _fieldOfView;
    float _zNear;
    float _zFar;
    float _orthoScale;

    glm::mat4 _cameraToViewportMatrix;
    glm::mat4 _worldToCameraMatrix;
    glm::mat4 _worldToViewportMatrix;
};

} // namespace Lattice
