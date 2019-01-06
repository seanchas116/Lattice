#include "ViewportRenderer.hpp"
#include "GridFloor.hpp"
#include "MeshRenderer.hpp"
#include "Operations.hpp"
#include "Manipulator.hpp"
#include "ManipulatorController.hpp"
#include "ItemPicker.hpp"
#include "ItemInteractor.hpp"
#include "../resource/Resource.hpp"
#include "../ui/AppState.hpp"
#include "../document/Document.hpp"
#include "../document/Item.hpp"
#include "../document/MeshItem.hpp"
#include "../support/Debug.hpp"

using namespace glm;

namespace Lattice::Viewport {

ViewportRenderer::ViewportRenderer(const SP<UI::AppState> &appState) {
    _appState = appState;

    initializeOpenGLFunctions();

    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    _operations = std::make_shared<Operations>();
    _gridFloor = std::make_shared<GridFloor>();
    _manipulator = std::make_shared<Manipulator>();
    _manipulatorController = std::make_shared<ManipulatorController>(_manipulator, appState);
    _itemPicker = std::make_shared<ItemPicker>();
    _itemInteractor = std::make_shared<ItemInteractor>(_itemPicker);

    connect(appState.get(), &UI::AppState::isVertexVisibleChanged, this, &ViewportRenderer::updateNeeded);
    connect(appState.get(), &UI::AppState::isEdgeVisibleChanged, this, &ViewportRenderer::updateNeeded);
    connect(appState.get(), &UI::AppState::isFaceVisibleChanged, this, &ViewportRenderer::updateNeeded);

    connect(appState.get(), &UI::AppState::isTranslateHandleVisibleChanged, this, &ViewportRenderer::updateNeeded);
    connect(appState.get(), &UI::AppState::isRotateHandleVisibleChanged, this, &ViewportRenderer::updateNeeded);
    connect(appState.get(), &UI::AppState::isScaleHandleVisibleChanged, this, &ViewportRenderer::updateNeeded);

    connect(appState.get(), &UI::AppState::isTranslateHandleVisibleChanged, _manipulator.get(), &Manipulator::setIsTranslateHandleVisible);
    connect(appState.get(), &UI::AppState::isRotateHandleVisibleChanged, _manipulator.get(), &Manipulator::setIsRotateHandleVisible);
    connect(appState.get(), &UI::AppState::isScaleHandleVisibleChanged, _manipulator.get(), &Manipulator::setIsScaleHandleVisible);

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
        auto meshItem = std::dynamic_pointer_cast<Document::MeshItem>(item);
        if (!meshItem) {
            return;
        }
        connect(meshItem.get(), &Document::MeshItem::locationChanged, this, &ViewportRenderer::updateNeeded);

        auto it = _meshRenderers.find(meshItem);
        if (it != _meshRenderers.end()) {
            newMeshRenderers[meshItem] = it->second;
            return;
        }

        auto renderer = std::make_shared<MeshRenderer>(meshItem);
        newMeshRenderers[meshItem] = renderer;
    });

    _meshRenderers = newMeshRenderers;

    _gridFloor->draw(_operations, _camera);

    if (_appState->isFaceVisible()) {
        for (auto& [item, renderer] : _meshRenderers) {
            renderer->drawFaces(_operations, _camera);
        }
    }

    if (_appState->isEdgeVisible()) {
        for (auto& [item, renderer] : _meshRenderers) {
            renderer->drawEdges(_operations, _camera);
        }
    }

    if (_appState->isVertexVisible()) {
        for (auto& [item, renderer] : _meshRenderers) {
            renderer->drawVertices(_operations, _camera);
        }
    }

    glClear(GL_DEPTH_BUFFER_BIT);

    if (_appState->document()->currentItem()) {
        _manipulator->draw(_operations, _camera);
    }
}

void ViewportRenderer::mousePress(QMouseEvent *event, dvec2 pos) {
    if (_manipulator->mousePress(event, pos, _camera)) {
        return;
    }
    if (_itemInteractor->mousePress(event, pos, _camera)) {
        return;
    }
}

void ViewportRenderer::mouseMove(QMouseEvent *event, dvec2 pos) {
    if (_manipulator->mouseMove(event, pos, _camera)) {
        return;
    }
    if (_itemInteractor->mouseMove(event, pos, _camera)) {
        return;
    }
}

void ViewportRenderer::mouseRelease(QMouseEvent *event, dvec2 pos) {
    if (_manipulator->mouseRelease(event, pos, _camera)) {
        return;
    }
    if (_itemInteractor->mouseRelease(event, pos, _camera)) {
        return;
    }
}

} // namespace Lattice
