#include "EditorScene.hpp"
#include "GridFloor.hpp"

namespace Lattice {
namespace Editor {

EditorScene::EditorScene() :
    _gridFloor(makeShared<GridFloor>())
{
}

void EditorScene::updateLayers() {
    _layers = {
        {_gridFloor},
    };
}

} // namespace Editor
} // namespace Lattice
