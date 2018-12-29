#include "ViewportRenderer.hpp"
#include "GridFloor.hpp"
#include "MeshRenderer.hpp"
#include "Operations.hpp"
#include "Manipulator.hpp"
#include "../resource/Resource.hpp"
#include "../app/AppState.hpp"
#include "../document/Document.hpp"
#include "../document/Item.hpp"
#include "../document/MeshItem.hpp"
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

namespace Lattice {

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
}

void ViewportRenderer::resize(ivec2 physicalSize, ivec2 logicalSize) {
    glViewport(0, 0, physicalSize.x, physicalSize.y);
    _logicalSize = logicalSize;
    _projection = Projection(logicalSize, glm::radians(60.f), 0.1f, 100.f);
}

void ViewportRenderer::render() {
    glClearColor(0.8f, 0.8f, 0.8f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    std::unordered_map<SP<MeshItem>, SP<MeshRenderer>> newMeshRenderers;

    _appState->document()->rootItem()->forEachDescendant([&] (auto& item) {
        auto meshItem = std::dynamic_pointer_cast<MeshItem>(item);
        if (!meshItem) {
            return;
        }
        connect(meshItem.get(), &MeshItem::locationChanged, this, &ViewportRenderer::updateNeeded);

        auto it = _meshRenderers.find(meshItem);
        if (it != _meshRenderers.end()) {
            newMeshRenderers[meshItem] = it->second;
            return;
        }

        auto renderer = std::make_shared<MeshRenderer>(meshItem);
        newMeshRenderers[meshItem] = renderer;
    });

    _meshRenderers = newMeshRenderers;

    _gridFloor->draw(_operations, _camera.matrix(), _projection);

    if (_appState->isFaceVisible()) {
        for (auto& [item, renderer] : _meshRenderers) {
            renderer->drawFaces(_operations, _camera.matrix(), _projection);
        }
    }

    if (_appState->isEdgeVisible()) {
        for (auto& [item, renderer] : _meshRenderers) {
            renderer->drawEdges(_operations, _camera.matrix(), _projection);
        }
    }

    if (_appState->isVertexVisible()) {
        for (auto& [item, renderer] : _meshRenderers) {
            renderer->drawVertices(_operations, _camera.matrix(), _projection);
        }
    }

    _manipulator->draw(_operations, _camera.matrix(), _projection);
}

} // namespace Lattice
