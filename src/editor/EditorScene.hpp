#pragma once
#include "../viewport/Renderable.hpp"
#include "../gl/ContextRecallable.hpp"
#include <QObject>

class QKeyEvent;

namespace Lattice {

namespace State {
class AppState;
}

namespace Document {
class MeshObject;
}

namespace Editor {

class Background;
class GridFloor;
class MeshRenderer;

namespace MeshEditor {
class MeshEditor;
}

namespace Manipulator {
class ObjectManipulator;
}

class EditorScene : public Viewport::Renderable, protected GL::ContextRecallable {
    Q_OBJECT
public:
    EditorScene(const SP<State::AppState>& appState);

    void updateRenderables();

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    SP<State::AppState> _appState;

    SP<Background> _background;
    SP<GridFloor> _gridFloor;
    SP<Manipulator::ObjectManipulator> _objectManipulator;
    std::unordered_map<SP<Document::MeshObject>, SP<MeshRenderer>> _meshRenderers;
    Opt<SP<MeshEditor::MeshEditor>> _meshEditor;
};

} // namespace Editor
} // namespace Lattice
