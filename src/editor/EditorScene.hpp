#pragma once
#include <QObject>
#include "../render/RenderableObject.hpp"

namespace Lattice {

namespace UI {
class AppState;
}

namespace Document {
class MeshItem;
}

namespace Editor {

class Background;
class GridFloor;
class MeshRenderer;
class MeshEditor;

namespace Manipulator {
class ObjectManipulator;
class MeshManipulator;
}

class EditorScene : public QObject {
    Q_OBJECT
public:
    EditorScene(const SP<UI::AppState>& appState);

    std::vector<SP<Render::Renderable>> updateRenderables();

signals:
    void updateRequested();

private:
    SP<UI::AppState> _appState;

    SP<Background> _background;
    SP<GridFloor> _gridFloor;
    SP<Manipulator::ObjectManipulator> _objectManipulator;
    SP<Manipulator::MeshManipulator> _meshManipulator;
    std::unordered_map<SP<Document::MeshItem>, SP<MeshRenderer>> _meshRenderers;
    Opt<SP<MeshEditor>> _editedMeshRenderers;
};

} // namespace Editor
} // namespace Lattice
