#include "Background.hpp"
#include "../document/Document.hpp"
#include <QtDebug>

namespace Lattice {
namespace Editor {

Background::Background(const SP<UI::AppState> &appState) : _appState(appState) {
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

std::optional<Render::HitResult> Background::hitTest(glm::dvec2 pos, const Camera &camera) const {
    Q_UNUSED(pos); Q_UNUSED(camera);
    Render::HitResult result;
    result.t = 1;
    return result;
}

void Background::mousePress(const Render::MouseEvent &event) {
    Q_UNUSED(event);
    _appState->document()->setSelectedItems({});
}

} // namespace Editor
} // namespace Lattice
