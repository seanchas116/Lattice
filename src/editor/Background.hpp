#pragma once
#include "../render/RenderableObject.hpp"
#include "../state/AppState.hpp"

namespace Lattice {
namespace Editor {

class Background : public Render::RenderableObject, protected QOpenGLExtraFunctions {
public:
    Background(const SP<State::AppState>& appState);
    void draw(const SP<Draw::Operations> &operations, const SP<Camera> &camera) override;
    void drawPickables(const SP<Draw::Operations> &operations, const SP<Camera> &camera) override;
    void mousePressEvent(const Render::MouseEvent &event) override;

private:
    SP<State::AppState> _appState;
};

} // namespace Editor
} // namespace Lattice
