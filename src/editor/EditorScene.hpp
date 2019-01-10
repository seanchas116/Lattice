#pragma once
#include <QObject>
#include "../render/RenderWidget.hpp"

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

namespace Manipulator {
class Controller;
}

class EditorScene : public QObject {
    Q_OBJECT
public:
    EditorScene(const SP<UI::AppState>& appState);

    auto& layers() const { return _layers; }

    void updateLayers();

signals:
    void updateRequested();

private:
    SP<UI::AppState> _appState;

    SP<Background> _background;
    SP<GridFloor> _gridFloor;
    SP<Manipulator::Controller> _manipulatorController;
    std::unordered_map<SP<Document::MeshItem>, SP<MeshRenderer>> _meshRenderers;

    std::vector<Render::Layer> _layers;
};

} // namespace Editor
} // namespace Lattice
