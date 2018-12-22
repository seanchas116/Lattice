#include "Projection.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace Lattice {

Projection::Projection() :
    Projection(glm::vec2(100), glm::radians(60.f), 0.1f, 100.f)
{
}

Projection::Projection(glm::vec2 viewSize, float fieldOfView, float near, float far) :
    _viewSize(viewSize),
    _fieldOfView(fieldOfView),
    _near(near),
    _far(far)
{
    _matrix = glm::perspective(fieldOfView, float(viewSize.x) / float(viewSize.y), near, far);
}

} // namespace Lattice
