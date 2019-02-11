#pragma once
#include <QOpenGLWidget>
#include "../support/Shorthands.hpp"
#include "../render/ViewportContainer.hpp"
#include "../ui/ViewportSplitMode.hpp"

namespace Lattice::UI {
class AppState;
}

namespace Lattice::Editor {

class KeyObserver;
class EditorScene;

class EditorViewportContainer final : public Render::ViewportContainer {
    Q_OBJECT
public:
    explicit EditorViewportContainer(const SP<UI::AppState>& appState, QWidget *parent = nullptr);

private:
    void setSplitMode(UI::ViewportSplitMode split);
    void setRenderables(const std::vector<SP<Render::Renderable>>& renderables);

    SP<UI::AppState> _appState;
    SP<KeyObserver> _keyObserver;
    Opt<SP<EditorScene>> _scene;
    std::vector<SP<Render::Renderable>> _renderables;
};

} // namespace Lattice
