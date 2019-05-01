#pragma once
#include "../viewport/RenderableObject.hpp"
#include "../state/AppState.hpp"

namespace Lattice {
namespace Editor {

class Background : public Viewport::RenderableObject, protected QOpenGLExtraFunctions {
public:
    Background(const SP<State::AppState>& appState);
    void draw(const SP<Draw::Operations> &operations, const SP<Camera> &camera) override;
    void drawPickables(const SP<Draw::Operations> &operations, const SP<Camera> &camera, const Viewport::PickableID &pickableID) override;
    void mousePressEvent(const Viewport::MouseEvent &event) override;

private:
    SP<State::AppState> _appState;
};

} // namespace Editor
} // namespace Lattice
