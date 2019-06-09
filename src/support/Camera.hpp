#pragma once
#include <glm/glm.hpp>

namespace Lattice {

class Camera {
public:
    enum class Projection {
        Perspective,
        Orthographic,
    };

    static Camera perspective(glm::mat4 viewportToWorldMatrix, glm::vec2 viewportSize, double fieldOfView, double zNear, double zFar);
    static Camera orthographic(glm::mat4 viewportToWorldMatrix, glm::vec2 viewportSize, double scale);

private:
    Projection _projection;
    glm::mat4 _viewportToWorldMatrix;
    glm::vec2 _viewportSize;
    double _fieldOfView;
    double _zNear;
    double _zFar;
    double _orthoScale;

    glm::mat4 _cameraToViewportMatrix;
    glm::mat4 _worldToCameraMatrix;
    glm::mat4 _worldToViewportMatrix;
};

} // namespace Lattice
