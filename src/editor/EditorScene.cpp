#include "EditorScene.hpp"
#include "Background.hpp"
#include "GridFloor.hpp"
#include "MeshRenderer.hpp"
#include "./mesheditor/MeshEditor.hpp"
#include "./manipulator/ObjectManipulator.hpp"
#include "./manipulator/MeshManipulator.hpp"
#include "../state/AppState.hpp"
#include "../document/Document.hpp"
#include "../document/Object.hpp"
#include "../document/MeshObject.hpp"

namespace Lattice {
namespace Editor {

EditorScene::EditorScene(const SP<State::AppState> &appState) :
    _appState(appState),
    _background(makeShared<Background>(appState)),
    _gridFloor(makeShared<GridFloor>()),
    _objectManipulator(makeShared<Manipulator::ObjectManipulator>(appState))
{
    connect(appState.get(), &State::AppState::isVertexSelectableChanged, this, &EditorScene::updateRenderables);
    connect(appState.get(), &State::AppState::isEdgeSelectableChanged, this, &EditorScene::updateRenderables);
    connect(appState.get(), &State::AppState::isFaceSelectableChanged, this, &EditorScene::updateRenderables);
    connect(appState.get(), &State::AppState::toolChanged, this, &EditorScene::updateRenderables);

    connect(appState.get(), &State::AppState::isTranslateHandleVisibleChanged, _objectManipulator.get(), &Manipulator::Manipulator::setTranslateHandleVisible);
    connect(appState.get(), &State::AppState::isRotateHandleVisibleChanged, _objectManipulator.get(), &Manipulator::Manipulator::setRotateHandleVisible);
    connect(appState.get(), &State::AppState::isScaleHandleVisibleChanged, _objectManipulator.get(), &Manipulator::Manipulator::setScaleHandleVisible);

    connect(appState->document().get(), &Document::Document::itemInserted, this, &EditorScene::updateRenderables);
    connect(appState->document().get(), &Document::Document::itemRemoved, this, &EditorScene::updateRenderables);
    connect(appState->document().get(), &Document::Document::selectedItemsChanged, this, &EditorScene::updateRenderables);
    connect(appState->document().get(), &Document::Document::editedItemChanged, this, &EditorScene::updateRenderables);
    connect(appState->document().get(), &Document::Document::meshSelectionChanged, this, &EditorScene::updateRenderables);

    updateRenderables();
}

void EditorScene::updateRenderables() {
    recallContext();

    std::unordered_map<SP<Document::MeshObject>, SP<MeshRenderer>> newMeshRenderers;

    auto editedItem = _appState->document()->editedItem();
    if (editedItem) {
        if (!_meshEditor || (*_meshEditor)->item() != editedItem) {
            _meshEditor = makeShared<MeshEditor::MeshEditor>(_appState, *editedItem);
        }
    } else {
        _meshEditor = std::nullopt;
    }

    _appState->document()->rootItem()->forEachDescendant([&] (auto& item) {
        LATTICE_OPTIONAL_GUARD(meshItem, dynamicPointerCast<Document::MeshObject>(item), return;)
                connect(meshItem.get(), &Document::MeshObject::locationChanged, this, [this] { update(); });
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

    std::vector<SP<Render::Renderable>> renderables;
    renderables.push_back(_background);

    renderables.push_back(_gridFloor);
    for (auto& [item, renderer] : _meshRenderers) {
        renderables.push_back(renderer);
    }
    if (_meshEditor) {
        renderables.push_back(*_meshEditor);
    }

    if (!_appState->document()->selectedItems().empty() && !_appState->document()->isEditing()) {
        renderables.push_back(_objectManipulator);
    }

    setChildRenderables(renderables);
    update();
}

void EditorScene::keyPressEvent(QKeyEvent *event) {
    if (_meshEditor) {
        (*_meshEditor)->keyPressEvent(event);
    }
}

void EditorScene::keyReleaseEvent(QKeyEvent *event) {
    if (_meshEditor) {
        (*_meshEditor)->keyReleaseEvent(event);
    }
}

} // namespace Editor
} // namespace Lattice
