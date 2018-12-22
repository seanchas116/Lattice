#include "ViewportRenderer.hpp"
#include "GridFloor.hpp"
#include "MeshRenderer.hpp"
#include "Shaders.hpp"
#include "../resource/Resource.hpp"
#include "../app/AppState.hpp"
#include "../document/Document.hpp"
#include "../document/Item.hpp"
#include "../document/MeshItem.hpp"
#include "../document/MeshShape.hpp"
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

namespace Lattice {

ViewportRenderer::ViewportRenderer(const SP<AppState> &appState) {
    _appState = appState;

    initializeOpenGLFunctions();

    _shaders = std::make_shared<Lattice::Shaders>();
    _gridFloor = std::make_shared<GridFloor>();
}

void ViewportRenderer::resize(ivec2 physicalSize, ivec2 logicalSize) {
    glViewport(0, 0, physicalSize.x, physicalSize.y);
    _logicalSize = logicalSize;
    _projection = Projection(logicalSize, glm::radians(60.f), 0.1f, 100.f);
}

void ViewportRenderer::render() {
    glClearColor(0.8f, 0.8f, 0.8f, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    _appState->document()->rootItem()->forEachDescendant([&] (auto& item) {
        auto meshItem = std::dynamic_pointer_cast<MeshItem>(item);
        if (!meshItem) { return; }
        auto renderer = std::make_shared<MeshRenderer>();
        renderer->update(meshItem->shape());
        _meshRenderers[meshItem] = renderer;
    });

    _gridFloor->draw(_shaders, _camera.matrix(), _projection);

    for (auto& [item, renderer] : _meshRenderers) {
        renderer->drawFaces(_shaders, _camera.matrix(), _projection);
    }

    for (auto& [item, renderer] : _meshRenderers) {
        renderer->drawEdges(_shaders, _camera.matrix(), _projection);
    }

    for (auto& [item, renderer] : _meshRenderers) {
        renderer->drawVertices(_shaders, _camera.matrix(), _projection);
    }
}

} // namespace Lattice
