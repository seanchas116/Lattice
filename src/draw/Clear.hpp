#pragma once

#include <QOpenGLExtraFunctions>
#include <glm/glm.hpp>

namespace Lattice {
namespace Draw {

class Clear final : protected QOpenGLExtraFunctions {
public:
    Clear();

    void draw(glm::vec4 color, float depth);
};

} // namespace Draw
} // namespace Lattice
