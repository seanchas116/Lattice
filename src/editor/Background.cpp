#include "Background.hpp"
#include "../document/Document.hpp"
#include <QtDebug>

namespace Lattice {
namespace Editor {

Background::Background(const SP<State::AppState> &appState) : _appState(appState) {
    initializeOpenGLFunctions();
}

void Background::draw(const SP<Render::Operations> &operations, const SP<Camera> &camera) {
    Q_UNUSED(operations); Q_UNUSED(camera);

    // TODO: manage depth test in better way
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glClearDepthf(1.f);
    glClearColor(0.8f, 0.8f, 0.8f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Background::drawPickables(const SP<Render::Operations> &operations, const SP<Camera> &camera) {
    Q_UNUSED(operations); Q_UNUSED(camera);

    auto color = toIDColor();
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Background::mousePressEvent(const Render::MouseEvent &event) {
    Q_UNUSED(event);
    _appState->document()->setSelectedObjects({});
    _appState->document()->setMeshSelection({});
}

} // namespace Editor
} // namespace Lattice
