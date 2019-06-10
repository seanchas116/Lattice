#include "Background.hpp"
#include "../document/Document.hpp"
#include <QtDebug>

namespace Lattice {
namespace Editor {

Background::Background(const SP<State::AppState> &appState) : _appState(appState) {
    initializeOpenGLFunctions();
}

void Background::draw(const Viewport::DrawEvent &event) {
    // TODO: manage depth test in better way
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    event.operations->clear.clear(glm::vec4(0.8, 0.8, 0.8, 1), 1);
}

void Background::drawHitArea(const Viewport::DrawEvent &event) {
    auto color = toIDColor();
    event.operations->clear.clear(color, 1);
}

void Background::mousePressEvent(const Viewport::MouseEvent &event) {
    Q_UNUSED(event);
    _appState->deselectAll();
}

} // namespace Editor
} // namespace Lattice
