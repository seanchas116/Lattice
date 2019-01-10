#include "ViewportRenderer.hpp"
#include "GridFloor.hpp"
#include "MeshRenderer.hpp"
#include "OldManipulator.hpp"
#include "OldManipulatorController.hpp"
#include "ItemPicker.hpp"
#include "ItemInteractor.hpp"
#include "../render/Operations.hpp"
#include "../resource/Resource.hpp"
#include "../ui/AppState.hpp"
#include "../document/Document.hpp"
#include "../document/Item.hpp"
#include "../document/MeshItem.hpp"
#include "../support/Debug.hpp"

using namespace glm;

namespace Lattice::Editor {

ViewportRenderer::ViewportRenderer(const SP<UI::AppState> &appState) :
    _appState(appState),
    _operations(makeShared<Render::Operations>()),
    _gridFloor(makeShared<GridFloor>()),
    _manipulator(makeShared<OldManipulator>()),
    _manipulatorController(makeShared<OldManipulatorController>(_manipulator, appState)),
    _itemPicker(makeShared<ItemPicker>()),
    _itemInteractor(makeShared<ItemInteractor>(_itemPicker))
{
    initializeOpenGLFunctions();

    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    connect(appState.get(), &UI::AppState::isVertexVisibleChanged, this, &ViewportRenderer::updateNeeded);
    connect(appState.get(), &UI::AppState::isEdgeVisibleChanged, this, &ViewportRenderer::updateNeeded);
    connect(appState.get(), &UI::AppState::isFaceVisibleChanged, this, &ViewportRenderer::updateNeeded);

    connect(appState.get(), &UI::AppState::isTranslateHandleVisibleChanged, this, &ViewportRenderer::updateNeeded);
    connect(appState.get(), &UI::AppState::isRotateHandleVisibleChanged, this, &ViewportRenderer::updateNeeded);
    connect(appState.get(), &UI::AppState::isScaleHandleVisibleChanged, this, &ViewportRenderer::updateNeeded);

    connect(appState.get(), &UI::AppState::isTranslateHandleVisibleChanged, _manipulator.get(), &OldManipulator::setIsTranslateHandleVisible);
    connect(appState.get(), &UI::AppState::isRotateHandleVisibleChanged, _manipulator.get(), &OldManipulator::setIsRotateHandleVisible);
    connect(appState.get(), &UI::AppState::isScaleHandleVisibleChanged, _manipulator.get(), &OldManipulator::setIsScaleHandleVisible);

    connect(appState->document().get(), &Document::Document::itemInserted, this, &ViewportRenderer::updateNeeded);
    connect(appState->document().get(), &Document::Document::currentItemChanged, this, &ViewportRenderer::updateNeeded);
}

void ViewportRenderer::resize(ivec2 physicalSize, ivec2 logicalSize) {
    glViewport(0, 0, physicalSize.x, physicalSize.y);
    _logicalSize = logicalSize;
    _camera.setViewSize(logicalSize);
}

void ViewportRenderer::render() {
    glClearDepthf(1.f);
    glClearColor(0.8f, 0.8f, 0.8f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _itemPicker->update(_appState->document()->rootItem());

    std::unordered_map<SP<Document::MeshItem>, SP<MeshRenderer>> newMeshRenderers;

    _appState->document()->rootItem()->forEachDescendant([&] (auto& item) {
        LATTICE_OPTIONAL_GUARD(meshItem, dynamicPointerCast<Document::MeshItem>(item), return;)
        connect(meshItem.get(), &Document::MeshItem::locationChanged, this, &ViewportRenderer::updateNeeded);

        auto it = _meshRenderers.find(meshItem);
        if (it != _meshRenderers.end()) {
            newMeshRenderers.insert({meshItem, it->second});
            return;
        }

        auto renderer = makeShared<MeshRenderer>(_appState, meshItem);
        newMeshRenderers.insert({meshItem, renderer});
    });

    _meshRenderers = newMeshRenderers;

    _gridFloor->draw(_operations, _camera);

    for (auto& [item, renderer] : _meshRenderers) {
        renderer->draw(_operations, _camera);
    }

    glClear(GL_DEPTH_BUFFER_BIT);

    if (_appState->document()->currentItem()) {
        _manipulator->draw(_operations, _camera);
    }
}

void ViewportRenderer::mousePress(QMouseEvent *event, dvec2 pos) {
    {
        auto [hit, t] = _manipulator->mousePress(event, pos, _camera);
        if (hit) {
            _manipulatorDragged = true;
            return;
        }
    }

    {
        auto [hit, t] = _itemInteractor->mousePress(event, pos, _camera);
        if (hit) {
            _itemInteractorDragged = true;
            return;
        }
    }
}

void ViewportRenderer::mouseMove(QMouseEvent *event, dvec2 pos) {
    if (_manipulatorDragged) {
        _manipulator->mouseMove(event, pos, _camera);
    }
    if (_itemInteractorDragged) {
        _itemInteractor->mouseMove(event, pos, _camera);
    }
}

void ViewportRenderer::mouseRelease(QMouseEvent *event, dvec2 pos) {
    if (_manipulatorDragged) {
        _manipulator->mouseRelease(event, pos, _camera);
    }
    if (_itemInteractorDragged) {
        _itemInteractor->mouseRelease(event, pos, _camera);
    }

    _manipulatorDragged = false;
    _itemInteractorDragged = false;
}

} // namespace Lattice
