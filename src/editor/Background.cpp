#include "Background.hpp"
#include "../document/Document.hpp"
#include <QtDebug>

namespace Lattice {
namespace Editor {

Background::Background(const SP<State::AppState> &appState) : _appState(appState) {
    initializeOpenGLFunctions();
}

void Background::draw(const SP<Draw::Operations> &operations, const SP<OldCamera> &camera) {
    Q_UNUSED(operations); Q_UNUSED(camera);

    // TODO: manage depth test in better way
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    operations->clear.clear(glm::vec4(0.8, 0.8, 0.8, 1), 1);
}

void Background::drawHitArea(const SP<Draw::Operations> &operations, const SP<OldCamera> &camera) {
    Q_UNUSED(operations); Q_UNUSED(camera);

    auto color = toIDColor();
    operations->clear.clear(color, 1);
}

void Background::mousePressEvent(const Viewport::MouseEvent &event) {
    Q_UNUSED(event);
    _appState->deselectAll();
}

} // namespace Editor
} // namespace Lattice
