#include "EditorScene.hpp"
#include "Background.hpp"
#include "GridFloor.hpp"
#include "MeshRenderer.hpp"
#include "MeshEditor.hpp"
#include "./manipulator/ObjectManipulator.hpp"
#include "./manipulator/MeshManipulator.hpp"
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
    _objectManipulator(makeShared<Manipulator::ObjectManipulator>(appState)),
    _meshManipulator(makeShared<Manipulator::MeshManipulator>(appState))
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
    connect(appState->document().get(), &Document::Document::meshSelectionChanged, this, &EditorScene::updateRequested);
}

std::vector<SP<Render::RenderableObject> > EditorScene::updateRenderables() {
    std::unordered_map<SP<Document::MeshItem>, SP<MeshRenderer>> newMeshRenderers;

    auto editedItem = _appState->document()->editedItem();
    if (editedItem) {
        if (!_editedMeshRenderers || (*_editedMeshRenderers)->item() != editedItem) {
            _editedMeshRenderers = makeShared<MeshEditor>(_appState, *editedItem);
        }
    } else {
        _editedMeshRenderers = std::nullopt;
    }

    _appState->document()->rootItem()->forEachDescendant([&] (auto& item) {
        LATTICE_OPTIONAL_GUARD(meshItem, dynamicPointerCast<Document::MeshItem>(item), return;)
        connect(meshItem.get(), &Document::MeshItem::locationChanged, this, &EditorScene::updateRequested);
        if (item == _appState->document()->editedItem()) {
            return;
        }

        auto it = _meshRenderers.find(meshItem);
        if (it != _meshRenderers.end()) {
            newMeshRenderers.insert({meshItem, it->second});
            return;
        }

        auto renderer = makeShared<MeshRenderer>(_appState, meshItem);
        newMeshRenderers.insert({meshItem, renderer});
    });

    _meshRenderers = newMeshRenderers;

    std::vector<SP<Render::RenderableObject>> renderables;
    renderables.push_back(_background);

    renderables.push_back(_gridFloor);
    for (auto& [item, renderer] : _meshRenderers) {
        renderables.push_back(renderer);
    }
    if (_editedMeshRenderers) {
        renderables.push_back(*_editedMeshRenderers);
    }

    if (_appState->document()->currentItem() && !_appState->document()->isEditing()) {
        for (auto& h : _objectManipulator->handles(_appState->isTranslateHandleVisible(), _appState->isRotateHandleVisible(), _appState->isScaleHandleVisible())) {
            renderables.push_back(h);
        }
    }
    if (_appState->document()->isEditing() && !_appState->document()->meshSelection().vertices.empty()) {
        for (auto& h : _meshManipulator->handles(_appState->isTranslateHandleVisible(), _appState->isRotateHandleVisible(), _appState->isScaleHandleVisible())) {
            renderables.push_back(h);
        }
    }

    for (auto& r : renderables) {
        connect(r.get(), &Render::RenderableObject::updateRequested, this, &EditorScene::updateRequested);
    }

    return renderables;
}

} // namespace Editor
} // namespace Lattice
