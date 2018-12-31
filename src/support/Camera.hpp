#pragma once
#include <optional>
#include <glm/glm.hpp>
#include "Location.hpp"

namespace Lattice {

class Camera {
public:
    Camera();

    Location location() const { return _location; }
    void setLocation(const Location &location);

    glm::vec2 viewSize() const { return _viewSize; }
    void setViewSize(const glm::vec2 &viewSize);

    float fieldOfView() const { return _fieldOfView; }
    void setFieldOfView(float fieldOfView);

    float zNear() const { return _zNear; }
    void setZNear(float zNear);

    float zFar() const { return _zFar; }
    void setZFar(float zFar);

    glm::mat4 worldToCameraMatrix() const { return _worldToCameraMatrix; }
    glm::mat4 cameraToScrenMatrix() const { return _cameraToScreenMatrix; }
    glm::mat4 worldToScreenMatrix() const { return _worldToScreenMatrix; }

    std::pair<glm::vec3, bool> worldToScreen(const glm::vec3& worldPos) const;
    glm::vec3 screenToWorld(const glm::vec3& screenPos) const;

private:
    void updateMatrix();

    Location _location;

    glm::vec2 _viewSize;
    float _fieldOfView;
    float _zNear;
    float _zFar;
    glm::mat4 _cameraToScreenMatrix;
    glm::mat4 _worldToCameraMatrix;
    glm::mat4 _worldToScreenMatrix;
};

} // namespace Lattice
