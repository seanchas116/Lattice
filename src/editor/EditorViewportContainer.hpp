#pragma once
#include <QOpenGLWidget>
#include "../support/Pointer.hpp"
#include "../render/ViewportContainer.hpp"

namespace Lattice::UI {
class AppState;
}

namespace Lattice::Editor {

class EditorScene;

class EditorViewportContainer final : public Render::ViewportContainer {
    Q_OBJECT
public:
    explicit EditorViewportContainer(const SP<UI::AppState>& appState, QWidget *parent = nullptr);

private:
    SP<UI::AppState> _appState;
    std::optional<SP<EditorScene>> _scene;
};

} // namespace Lattice
