#include "ViewportRenderer.hpp"
#include "GridFloor.hpp"
#include "MeshRenderer.hpp"
#include "Operations.hpp"
#include "Manipulator.hpp"
#include "ManipulatorController.hpp"
#include "../resource/Resource.hpp"
#include "../ui/AppState.hpp"
#include "../document/Document.hpp"
#include "../document/Item.hpp"
#include "../document/MeshItem.hpp"

using namespace glm;

namespace Lattice::Viewport {

ViewportRenderer::ViewportRenderer(const SP<AppState> &appState) {
    _appState = appState;

    initializeOpenGLFunctions();

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    _operations = std::make_shared<Operations>();
    _gridFloor = std::make_shared<GridFloor>();
    _manipulator = std::make_shared<Manipulator>();
    _manipulatorController = std::make_shared<ManipulatorController>(_manipulator, appState);

    connect(appState.get(), &AppState::isVertexVisibleChanged, this, &ViewportRenderer::updateNeeded);
    connect(appState.get(), &AppState::isEdgeVisibleChanged, this, &ViewportRenderer::updateNeeded);
    connect(appState.get(), &AppState::isFaceVisibleChanged, this, &ViewportRenderer::updateNeeded);
}

void ViewportRenderer::resize(ivec2 physicalSize, ivec2 logicalSize) {
    glViewport(0, 0, physicalSize.x, physicalSize.y);
    _logicalSize = logicalSize;
    _camera.setViewSize(logicalSize);
}

void ViewportRenderer::render() {
    glClearColor(0.8f, 0.8f, 0.8f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

    _manipulator->draw(_operations, _camera);
}

void ViewportRenderer::mousePress(QMouseEvent *event, dvec2 pos) {
    _manipulator->mousePress(event, pos, _camera);
}

void ViewportRenderer::mouseMove(QMouseEvent *event, dvec2 pos) {
    _manipulator->mouseMove(event, pos, _camera);
}

void ViewportRenderer::mouseRelease(QMouseEvent *event, dvec2 pos) {
    _manipulator->mouseRelease(event, pos, _camera);
}

} // namespace Lattice
