#pragma once
#include "../viewport/Renderable.hpp"
#include "../state/AppState.hpp"

namespace Lattice {
namespace Editor {

class Background : public Viewport::Renderable, protected QOpenGLExtraFunctions {
public:
    Background(const SP<State::AppState>& appState);
    void draw(const Viewport::DrawEvent &event) override;
    void drawHitArea(const Viewport::DrawEvent &event) override;
    void mousePressEvent(const Viewport::MouseEvent &event) override;

private:
    SP<State::AppState> _appState;
};

} // namespace Editor
} // namespace Lattice
