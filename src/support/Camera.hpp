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

    glm::mat4 viewMatrix() const { return _viewMatrix; }
    glm::mat4 projectionMatrix() const { return _projectionMatrix; }
    glm::mat4 viewProjectionMatrix() const { return _viewProjectionMatrix; }

    std::pair<glm::vec3, bool> project(const glm::vec3& worldPos) const;
    glm::vec3 unProject(const glm::vec3& screenPos) const;

private:
    void updateMatrix();

    Location _location;

    glm::vec2 _viewSize;
    float _fieldOfView;
    float _zNear;
    float _zFar;
    glm::mat4 _projectionMatrix;
    glm::mat4 _viewMatrix;
    glm::mat4 _viewProjectionMatrix;
};

} // namespace Lattice
