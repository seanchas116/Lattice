#include "ViewportRenderer.hpp"

namespace Lattice {

ViewportRenderer::ViewportRenderer() {
    initializeOpenGLFunctions();
}

void ViewportRenderer::resize(glm::ivec2 size) {
    glViewport(0, 0, size.x, size.y);
}

void ViewportRenderer::render() {
    glClearColor(1, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}

} // namespace Lattice
