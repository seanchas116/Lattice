#include "Background.hpp"

namespace Lattice {
namespace Editor {

Background::Background() {
    initializeOpenGLFunctions();
}

void Background::draw(const SP<Render::Operations> &operations, const Camera &camera) {
    Q_UNUSED(operations); Q_UNUSED(camera);

    // TODO: manage depth test in better way
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glClearDepthf(1.f);
    glClearColor(0.8f, 0.8f, 0.8f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

} // namespace Editor
} // namespace Lattice
