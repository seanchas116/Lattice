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
class Operations;
class OldManipulator;
class OldManipulatorController;
class ManipulatorController;
class ItemPicker;
class ItemInteractor;

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
    SP<OldManipulator> _manipulator;
    SP<OldManipulatorController> _oldManipulatorController;
    SP<ManipulatorController> _manipulatorController;
    std::unordered_map<SP<Document::MeshItem>, SP<MeshRenderer>> _meshRenderers;
    SP<ItemPicker> _itemPicker;
    SP<ItemInteractor> _itemInteractor;

    std::vector<Render::Layer> _layers;
};

} // namespace Editor
} // namespace Lattice
