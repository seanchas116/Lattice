#pragma once
#include <QOpenGLWidget>
#include "../support/Shorthands.hpp"
#include "../render/ViewportContainer.hpp"
#include "../state/ViewportSplitMode.hpp"

namespace Lattice {

namespace State {
class AppState;
}

namespace Editor {

class KeyObserver;
class EditorScene;

class EditorViewportContainer final : public Render::ViewportContainer {
    Q_OBJECT
public:
    explicit EditorViewportContainer(const SP<State::AppState>& appState, QWidget *parent = nullptr);
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    void setSplitMode(State::ViewportSplitMode split);
    void setRenderable(const Opt<SP<Render::Renderable>>& renderable);

    SP<State::AppState> _appState;
    SP<KeyObserver> _keyObserver;
    Opt<SP<EditorScene>> _scene;
    Opt<SP<Render::Renderable>> _renderable;
};

}
} // namespace Lattice
