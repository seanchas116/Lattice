#include "Projection.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace Lattice {

Projection::Projection() :
    Projection(glm::vec2(100), glm::radians(60.f), 0.1f, 100.f)
{
}

Projection::Projection(glm::vec2 viewSize, float fieldOfView, float zNear, float zFar) :
    _viewSize(viewSize),
    _fieldOfView(fieldOfView),
    _zNear(zNear),
    _zFar(zFar)
{
    _matrix = glm::perspective(fieldOfView, float(viewSize.x) / float(viewSize.y), zNear, zFar);
}

} // namespace Lattice
