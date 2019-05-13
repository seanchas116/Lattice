#include "Clear.hpp"

namespace Lattice {
namespace Draw {

Clear::Clear() {
    initializeOpenGLFunctions();
}

void Clear::draw(glm::vec4 color, float depth) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClearDepthf(depth);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

} // namespace Draw
} // namespace Lattice
