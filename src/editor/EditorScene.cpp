#include "EditorScene.hpp"
#include "GridFloor.hpp"
#include "Background.hpp"

namespace Lattice {
namespace Editor {

EditorScene::EditorScene() :
    _background(makeShared<Background>()),
    _gridFloor(makeShared<GridFloor>())
{
}

void EditorScene::updateLayers() {
    _layers = {
        {_background},
        {_gridFloor},
    };
}

} // namespace Editor
} // namespace Lattice
