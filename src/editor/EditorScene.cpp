#include "EditorScene.hpp"
#include "Background.hpp"
#include "GridFloor.hpp"
#include "MeshRenderer.hpp"
#include "./manipulator/Controller.hpp"
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
    _manipulatorController(makeShared<Manipulator::Controller>(appState))
{
    connect(appState.get(), &UI::AppState::isVertexVisibleChanged, this, &EditorScene::updateRequested);
    connect(appState.get(), &UI::AppState::isEdgeVisibleChanged, this, &EditorScene::updateRequested);
    connect(appState.get(), &UI::AppState::isFaceVisibleChanged, this, &EditorScene::updateRequested);

    connect(appState.get(), &UI::AppState::isTranslateHandleVisibleChanged, this, &EditorScene::updateRequested);
    connect(appState.get(), &UI::AppState::isRotateHandleVisibleChanged, this, &EditorScene::updateRequested);
    connect(appState.get(), &UI::AppState::isScaleHandleVisibleChanged, this, &EditorScene::updateRequested);

    connect(appState->document().get(), &Document::Document::itemInserted, this, &EditorScene::updateRequested);
    connect(appState->document().get(), &Document::Document::currentItemChanged, this, &EditorScene::updateRequested);
}

void EditorScene::updateLayers() {
    std::unordered_map<SP<Document::MeshItem>, SP<MeshRenderer>> newMeshRenderers;

    _appState->document()->rootItem()->forEachDescendant([&] (auto& item) {
        LATTICE_OPTIONAL_GUARD(meshItem, dynamicPointerCast<Document::MeshItem>(item), return;)
        connect(meshItem.get(), &Document::MeshItem::locationChanged, this, &EditorScene::updateRequested);

        auto it = _meshRenderers.find(meshItem);
        if (it != _meshRenderers.end()) {
            newMeshRenderers.insert({meshItem, it->second});
            return;
        }

        auto renderer = makeShared<MeshRenderer>(_appState, meshItem);
        newMeshRenderers.insert({meshItem, renderer});
    });

    _meshRenderers = newMeshRenderers;

    Render::Layer objectsLayer;
    objectsLayer.push_back(_gridFloor);
    for (auto& [item, renderer] : _meshRenderers) {
        objectsLayer.push_back(renderer);
    }

    Render::Layer handles;
    if (_appState->isRotateHandleVisible()) {
        for (auto& h : _manipulatorController->rotateHandles()) {
            handles.push_back(h);
        }
    }
    if (_appState->isScaleHandleVisible()) {
        for (auto& h : _manipulatorController->scaleHandles()) {
            handles.push_back(h);
        }
    }
    if (_appState->isTranslateHandleVisible()) {
        for (auto& h : _manipulatorController->translateHandles()) {
            handles.push_back(h);
        }
    }

    _layers = {
        {_background},
        objectsLayer,
        handles,
    };
}

} // namespace Editor
} // namespace Lattice
