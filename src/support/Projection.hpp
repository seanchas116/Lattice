#pragma once
#include <optional>
#include <glm/glm.hpp>

namespace Lattice {

class Projection {
public:
    Projection();

    glm::vec2 viewSize() const { return _viewSize; }
    void setViewSize(const glm::vec2 &viewSize);

    float fieldOfView() const { return _fieldOfView; }
    void setFieldOfView(float fieldOfView);

    float zNear() const { return _zNear; }
    void setZNear(float zNear);

    float zFar() const { return _zFar; }
    void setZFar(float zFar);

    glm::mat4 projectionMatrix() const { return _projectionMatrix; }

    std::pair<glm::vec3, bool> project(const glm::vec3& pos) const;
    glm::vec3 unProject(const glm::vec3& screenPos) const;

private:
    void updateMatrix();

    glm::vec2 _viewSize;
    float _fieldOfView;
    float _zNear;
    float _zFar;
    glm::mat4 _projectionMatrix;
};

} // namespace Lattice
