#pragma once
#include <glm/glm.hpp>

namespace Lattice {

class Camera {
public:
    enum class Projection {
        Perspective,
        Orthographic,
    };

    static Camera perspective(glm::mat4 location, glm::vec2 viewportSize, double fieldOfView, double zNear, double zFar);
    static Camera orthographic(glm::mat4 location, glm::vec2 viewportSize, double scale);
};

} // namespace Lattice
