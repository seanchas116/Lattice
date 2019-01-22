#pragma once
#include <QOpenGLWidget>
#include "../support/Pointer.hpp"
#include "../render/ViewportContainer.hpp"
#include "../ui/ViewportSplit.hpp"

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
    void setSplitMode(UI::ViewportSplit split);
    void setRenderables(const std::vector<SP<Render::Renderable>>& renderables);

    SP<UI::AppState> _appState;
    SP<KeyObserver> _keyObserver;
    std::optional<SP<EditorScene>> _scene;
    std::vector<SP<Render::Renderable>> _renderables;
};

} // namespace Lattice
