#include "ViewportRenderer.hpp"

namespace Lattice {

ViewportRenderer::ViewportRenderer() {
    initializeOpenGLFunctions();
}

void ViewportRenderer::resize(glm::ivec2 size) {
    glViewport(0, 0, size.x, size.y);
}

void ViewportRenderer::render() {
    glClearColor(0.8f, 0.8f, 0.8f, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}

} // namespace Lattice
