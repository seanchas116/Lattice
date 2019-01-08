#include "EditorScene.hpp"
#include "Background.hpp"
#include "GridFloor.hpp"
#include "MeshRenderer.hpp"
#include "Manipulator.hpp"
#include "ManipulatorController.hpp"
#include "ItemPicker.hpp"
#include "ItemInteractor.hpp"

namespace Lattice {
namespace Editor {

EditorScene::EditorScene(const SP<UI::AppState> &appState) :
    _appState(appState),
    _background(makeShared<Background>()),
    _gridFloor(makeShared<GridFloor>()),
    _manipulator(makeShared<Manipulator>()),
    _manipulatorController(makeShared<ManipulatorController>(_manipulator, appState)),
    _itemPicker(makeShared<ItemPicker>()),
    _itemInteractor(makeShared<ItemInteractor>(_itemPicker))
{
}

void EditorScene::updateLayers() {
    _layers = {
        {_background},
        {_gridFloor},
        {_manipulator},
    };
}

} // namespace Editor
} // namespace Lattice
