#include "EditorScene.hpp"
#include "Background.hpp"
#include "GridFloor.hpp"
#include "MeshRenderer.hpp"
#include "OldManipulator.hpp"
#include "OldManipulatorController.hpp"
#include "./manipulator/ManipulatorController.hpp"
#include "ItemPicker.hpp"
#include "ItemInteractor.hpp"
#include "../ui/AppState.hpp"
#include "../document/Document.hpp"
#include "../document/Item.hpp"
#include "../document/MeshItem.hpp"

namespace Lattice {
namespace Editor {

EditorScene::EditorScene(const SP<UI::AppState> &appState) :
    _appState(appState),
    _background(makeShared<Background>()),
    _gridFloor(makeShared<GridFloor>()),
    _manipulator(makeShared<OldManipulator>()),
    _oldManipulatorController(makeShared<OldManipulatorController>(_manipulator, appState)),
    _manipulatorController(makeShared<Manipulator::ManipulatorController>(appState)),
    _itemPicker(makeShared<ItemPicker>()),
    _itemInteractor(makeShared<ItemInteractor>(_itemPicker))
{
    connect(appState.get(), &UI::AppState::isVertexVisibleChanged, this, &EditorScene::updateRequested);
    connect(appState.get(), &UI::AppState::isEdgeVisibleChanged, this, &EditorScene::updateRequested);
    connect(appState.get(), &UI::AppState::isFaceVisibleChanged, this, &EditorScene::updateRequested);

    connect(appState.get(), &UI::AppState::isTranslateHandleVisibleChanged, this, &EditorScene::updateRequested);
    connect(appState.get(), &UI::AppState::isRotateHandleVisibleChanged, this, &EditorScene::updateRequested);
    connect(appState.get(), &UI::AppState::isScaleHandleVisibleChanged, this, &EditorScene::updateRequested);

    connect(appState.get(), &UI::AppState::isTranslateHandleVisibleChanged, _manipulator.get(), &OldManipulator::setIsTranslateHandleVisible);
    connect(appState.get(), &UI::AppState::isRotateHandleVisibleChanged, _manipulator.get(), &OldManipulator::setIsRotateHandleVisible);
    connect(appState.get(), &UI::AppState::isScaleHandleVisibleChanged, _manipulator.get(), &OldManipulator::setIsScaleHandleVisible);

    connect(appState->document().get(), &Document::Document::itemInserted, this, &EditorScene::updateRequested);
    connect(appState->document().get(), &Document::Document::currentItemChanged, this, &EditorScene::updateRequested);
}

void EditorScene::updateLayers() {
    _itemPicker->update(_appState->document()->rootItem());

    std::unordered_map<SP<Document::MeshItem>, SP<MeshRenderer>> newMeshRenderers;

    _appState->document()->rootItem()->forEachDescendant([&] (auto& item) {
        LATTICE_OPTIONAL_GUARD(meshItem, dynamicPointerCast<Document::MeshItem>(item), return;)
        connect(meshItem.get(), &Document::MeshItem::locationChanged, this, &EditorScene::updateRequested);

        auto it = _meshRenderers.find(meshItem);
        if (it != _meshRenderers.end()) {
            newMeshRenderers.insert({meshItem, it->second});
            return;
        }

        auto renderer = makeShared<MeshRenderer>(meshItem);
        newMeshRenderers.insert({meshItem, renderer});
    });

    _meshRenderers = newMeshRenderers;

    Render::Layer objectsLayer;
    objectsLayer.push_back(_gridFloor);
    for (auto& [item, renderer] : _meshRenderers) {
        if (_appState->isFaceVisible()) {
            objectsLayer.push_back(renderer->facesRenderer());
        }
        if (_appState->isEdgeVisible()) {
            objectsLayer.push_back(renderer->edgesRenderer());
        }
        if (_appState->isVertexVisible()) {
            objectsLayer.push_back(renderer->verticesRenderer());
        }
    }

    _layers = {
        {_background},
        objectsLayer,
        {_itemInteractor},
        //{_manipulator},
        _manipulatorController->translateHandles()
    };
}

} // namespace Editor
} // namespace Lattice
