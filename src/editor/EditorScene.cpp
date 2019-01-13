#include "EditorScene.hpp"
#include "Background.hpp"
#include "GridFloor.hpp"
#include "MeshRenderer.hpp"
#include "EditedMeshRenderer.hpp"
#include "./manipulator/Controller.hpp"
#include "../ui/AppState.hpp"
#include "../document/Document.hpp"
#include "../document/Item.hpp"
#include "../document/MeshItem.hpp"

namespace Lattice {
namespace Editor {

EditorScene::EditorScene(const SP<UI::AppState> &appState) :
    _appState(appState),
    _background(makeShared<Background>(appState)),
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
    connect(appState->document().get(), &Document::Document::itemAboutToBeRemoved, this, &EditorScene::updateRequested);
    connect(appState->document().get(), &Document::Document::currentItemChanged, this, &EditorScene::updateRequested);
    connect(appState->document().get(), &Document::Document::editedItemChanged, this, &EditorScene::updateRequested);
}

std::vector<SP<Render::Renderable> > EditorScene::updateRenderables() {
    std::unordered_map<SP<Document::MeshItem>, SP<MeshRenderer>> newMeshRenderers;

    auto editedItem = _appState->document()->editedItem();
    if (editedItem) {
        _editedMeshRenderers = makeShared<EditedMeshRenderer>(_appState, *editedItem);
    } else {
        _editedMeshRenderers = std::nullopt;
    }

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

    std::vector<SP<Render::Renderable>> renderables;
    renderables.push_back(_background);

    renderables.push_back(_gridFloor);
    for (auto& [item, renderer] : _meshRenderers) {
        if (_editedMeshRenderers && (*_editedMeshRenderers)->item() == item) {
            renderables.push_back(*_editedMeshRenderers);
        } else {
            renderables.push_back(renderer);
        }
    }

    if (_appState->document()->currentItem()) {
        if (_appState->isRotateHandleVisible()) {
            for (auto& h : _manipulatorController->rotateHandles()) {
                renderables.push_back(h);
            }
        }
        if (_appState->isScaleHandleVisible()) {
            for (auto& h : _manipulatorController->scaleHandles()) {
                renderables.push_back(h);
            }
        }
        if (_appState->isTranslateHandleVisible()) {
            for (auto& h : _manipulatorController->translateHandles()) {
                renderables.push_back(h);
            }
        }
    }

    return renderables;
}

} // namespace Editor
} // namespace Lattice
