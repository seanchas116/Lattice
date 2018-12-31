#pragma once
#include <optional>
#include <glm/glm.hpp>

namespace Lattice {

class Projection {
public:
    Projection();
    // TODO: support orthogonal projection
    Projection(glm::vec2 viewSize, float fieldOfView, float zNear, float zFar);

    glm::vec2 viewSize() const { return _viewSize; }
    float fieldOfView() const { return _fieldOfView; }
    float zNear() const { return _zNear; }
    float zFar() const { return _zFar; }

    glm::mat4 matrix() const { return _matrix; }

    std::pair<glm::vec3, bool> project(const glm::vec3& pos) const;
    glm::vec3 unProject(const glm::vec3& screenPos) const;

private:
    glm::vec2 _viewSize;
    float _fieldOfView;
    float _zNear;
    float _zFar;
    glm::mat4 _matrix;
};

} // namespace Lattice
