#include "DrawMethod.hpp"

namespace Lattice {
namespace GL {

DrawMethod::DrawMethod() {
    initializeOpenGLFunctions();
}

DrawMethod::~DrawMethod() {
}

void DrawMethod::bind() {
}

void DrawMethod::unbind() {
}

void PointsDrawMethod::draw() {
    glDrawArrays(GL_POINTS, GLint(_first), GLsizei(_count));
}

} // namespace GL
} // namespace Lattice
