#include "ViewportRenderer.hpp"
#include "../gl/Shader.hpp"
#include "../resource/Resource.hpp"

namespace Lattice {

ViewportRenderer::ViewportRenderer() {
    initializeOpenGLFunctions();
    _lineShader = std::make_shared<Shader>(readResource("src/viewport/ThickLine.vert"), readResource("src/viewport/ThickLine.geom"), readResource("src/viewport/ThickLine.frag"));
}

void ViewportRenderer::resize(glm::ivec2 size) {
    glViewport(0, 0, size.x, size.y);
}

void ViewportRenderer::render() {
    glClearColor(0.8f, 0.8f, 0.8f, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}

} // namespace Lattice
