#pragma once
#include <glm/glm.hpp>

namespace Lattice {

class Projection {
public:
    Projection();
    // TODO: support orthogonal projection
    Projection(glm::vec2 viewSize, float fieldOfView, float near, float far);

    glm::vec2 viewSize() const { return _viewSize; }
    float fieldOfView() const { return _fieldOfView; }
    float near() const { return _near; }
    float far() const { return _far; }

    glm::mat4 matrix() const { return _matrix; }

private:
    glm::vec2 _viewSize;
    float _fieldOfView;
    float _near;
    float _far;
    glm::mat4 _matrix;
};

} // namespace Lattice
