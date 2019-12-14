#pragma once
#include "../state/ViewportSplitMode.hpp"
#include "../support/Shorthands.hpp"
#include "../viewport/ViewportContainer.hpp"
#include <QOpenGLWidget>

namespace Lattice {

namespace State {
class AppState;
}

namespace Viewport {
class Renderable;
}

namespace Editor {

class KeyObserver;
class EditorScene;

class EditorViewportContainer final : public Viewport::ViewportContainer {
    Q_OBJECT
  public:
    explicit EditorViewportContainer(const SP<State::AppState> &appState, QWidget *parent = nullptr);
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

  private:
    void setSplitMode(State::ViewportSplitMode split);
    void setRenderable(const Opt<SP<Lattice::Viewport::Renderable>> &renderable);

    SP<State::AppState> _appState;
    SP<KeyObserver> _keyObserver;
    Opt<SP<EditorScene>> _scene;
    Opt<SP<Viewport::Renderable>> _renderable;
};

} // namespace Editor
} // namespace Lattice
