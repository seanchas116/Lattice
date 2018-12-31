#include "Projection.hpp"
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

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

std::pair<glm::vec3, bool> Projection::project(const glm::vec3 &pos) const {
    vec4 pos_clipSpace = _matrix * vec4(pos, 1);
    if (fabs(pos_clipSpace.x) <= pos_clipSpace.w && fabs(pos_clipSpace.y) <= pos_clipSpace.w && fabs(pos_clipSpace.z) <= pos_clipSpace.w) {
        vec3 ndc = vec3(pos_clipSpace.xyz) / pos_clipSpace.w;
        return {vec3((vec2(ndc.xy) + 1.f) * 0.5f * vec2(_viewSize), ndc.z), true};
    }
    return {vec3(0), false};
}

glm::vec3 Projection::unProject(const glm::vec3 &screenPos) const {
    return glm::unProject(screenPos, mat4(1), _matrix, vec4(0, 0, _viewSize));
}

} // namespace Lattice
