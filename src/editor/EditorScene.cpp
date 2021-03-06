#include "EditorScene.hpp"
#include "../document/Document.hpp"
#include "../document/MeshObject.hpp"
#include "../document/Object.hpp"
#include "../state/AppState.hpp"
#include "../state/MeshEditState.hpp"
#include "./manipulator/ObjectManipulator.hpp"
#include "./mesheditor/MeshEditor.hpp"
#include "Background.hpp"
#include "GridFloor.hpp"
#include "MeshRenderer.hpp"

namespace Lattice {
namespace Editor {

EditorScene::EditorScene(const SP<State::AppState> &appState) : _appState(appState),
                                                                _background(makeShared<Background>(appState)),
                                                                _gridFloor(makeShared<GridFloor>()),
                                                                _objectManipulator(makeShared<Manipulator::ObjectManipulator>(appState)) {
    connect(appState.get(), &State::AppState::isVertexSelectableChanged, this, &EditorScene::updateRenderables);
    connect(appState.get(), &State::AppState::isEdgeSelectableChanged, this, &EditorScene::updateRenderables);
    connect(appState.get(), &State::AppState::isFaceSelectableChanged, this, &EditorScene::updateRenderables);
    connect(appState.get(), &State::AppState::toolChanged, this, &EditorScene::updateRenderables);
    connect(appState.get(), &State::AppState::meshEditStateChanged, this, &EditorScene::updateRenderables);

    connect(appState.get(), &State::AppState::isTranslateHandleVisibleChanged, _objectManipulator.get(), &Manipulator::Manipulator::setTranslateHandleVisible);
    connect(appState.get(), &State::AppState::isRotateHandleVisibleChanged, _objectManipulator.get(), &Manipulator::Manipulator::setRotateHandleVisible);
    connect(appState.get(), &State::AppState::isScaleHandleVisibleChanged, _objectManipulator.get(), &Manipulator::Manipulator::setScaleHandleVisible);

    connect(appState->document().get(), &Document::Document::objectInserted, this, &EditorScene::updateRenderables);
    connect(appState->document().get(), &Document::Document::objectRemoved, this, &EditorScene::updateRenderables);
    connect(appState->document().get(), &Document::Document::selectedObjectsChanged, this, &EditorScene::updateRenderables);

    updateRenderables();
}

void EditorScene::updateRenderables() {
    recallContext();

    std::unordered_map<SP<Document::MeshObject>, SP<MeshRenderer>> newMeshRenderers;

    auto meshEditState = _appState->meshEditState();
    if (meshEditState) {
        if (!_meshEditor || (*_meshEditor)->meshEditState() != meshEditState) {
            _meshEditor = makeShared<MeshEditor::MeshEditor>(_appState, *meshEditState);
        }
    } else {
        _meshEditor = std::nullopt;
    }

    _appState->document()->rootObject()->forEachDescendant([&](auto &object) {
        LATTICE_OPTIONAL_GUARD(meshObject, dynamicPointerCast<Document::MeshObject>(object), return;)
        if (meshEditState && meshObject == (*meshEditState)->object()) {
            return;
        }
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
    for (auto &[object, renderer] : _meshRenderers) {
        renderables.push_back(renderer);
    }
    if (_meshEditor) {
        renderables.push_back(*_meshEditor);
    }

    if (!_appState->document()->selectedObjects().empty() && !_appState->meshEditState()) {
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
