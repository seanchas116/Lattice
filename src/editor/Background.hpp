#pragma once
#include "../render/Renderable.hpp"
#include "../ui/AppState.hpp"

namespace Lattice {

namespace Editor {

class Background : public Render::Renderable, protected QOpenGLExtraFunctions {
public:
    Background(const SP<UI::AppState>& appState);
    void draw(const SP<Render::Operations> &operations, const SP<Camera> &camera) override;
    std::optional<Render::HitResult> hitTest(glm::dvec2 pos, const SP<Camera> &camera) const override;
    void mousePress(const Render::MouseEvent &event) override;

private:
    SP<UI::AppState> _appState;
};

} // namespace Editor
} // namespace Lattice
