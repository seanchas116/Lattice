#include "EditorScene.hpp"
#include "Background.hpp"
#include "GridFloor.hpp"
#include "MeshRenderer.hpp"
#include "./oldmesheditor/MeshEditor.hpp"
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

    connect(appState->document().get(), &Document::Document::objectInserted, this, &EditorScene::updateRenderables);
    connect(appState->document().get(), &Document::Document::objectRemoved, this, &EditorScene::updateRenderables);
    connect(appState->document().get(), &Document::Document::selectedObjectsChanged, this, &EditorScene::updateRenderables);
    connect(appState->document().get(), &Document::Document::editedObjectChanged, this, &EditorScene::updateRenderables);
    connect(appState->document().get(), &Document::Document::meshSelectionChanged, this, &EditorScene::updateRenderables);

    updateRenderables();
}

void EditorScene::updateRenderables() {
    recallContext();

    std::unordered_map<SP<Document::MeshObject>, SP<MeshRenderer>> newMeshRenderers;

    auto editedObject = _appState->document()->editedObject();
    if (editedObject) {
        if (!_meshEditor || (*_meshEditor)->object() != editedObject) {
            _meshEditor = makeShared<OldMeshEditor::MeshEditor>(_appState, *editedObject);
        }
    } else {
        _meshEditor = std::nullopt;
    }

    _appState->document()->rootObject()->forEachDescendant([&] (auto& object) {
        LATTICE_OPTIONAL_GUARD(meshObject, dynamicPointerCast<Document::MeshObject>(object), return;)
        connect(meshObject.get(), &Document::MeshObject::locationChanged, this, [this] { emit updated(); });

        auto it = _meshRenderers.find(meshObject);
        if (it != _meshRenderers.end()) {
            newMeshRenderers.insert({meshObject, it->second});
            return;
        }

        auto renderer = makeShared<MeshRenderer>(_appState, meshObject);
        newMeshRenderers.insert({meshObject, renderer});
    });

    _meshRenderers = newMeshRenderers;

    std::vector<SP<Viewport::Renderable>> renderables;
    renderables.push_back(_background);

    renderables.push_back(_gridFloor);
    for (auto& [object, renderer] : _meshRenderers) {
        renderables.push_back(renderer);
    }
    if (_meshEditor) {
        renderables.push_back(*_meshEditor);
    }

    if (!_appState->document()->selectedObjects().empty() && !_appState->document()->isEditing()) {
        renderables.push_back(_objectManipulator);
    }

    setChildRenderables(renderables);
    emit updated();
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
