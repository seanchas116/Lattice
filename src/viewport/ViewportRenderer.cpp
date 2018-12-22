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

    _shaders = std::make_shared<Shaders>();
    _gridFloor = std::make_shared<GridFloor>();
}

void ViewportRenderer::resize(ivec2 physicalSize, ivec2 logicalSize) {
    glViewport(0, 0, physicalSize.x, physicalSize.y);
    _logicalSize = logicalSize;
    _projection = Projection(physicalSize, glm::radians(60.f), 0.1f, 100.f);
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

    auto MVP = _projection.matrix() * _camera.matrix();

    _shaders->thickLineShader.bind();
    _shaders->thickLineShader.setMVPMatrix(MVP);
    _shaders->thickLineShader.setViewportSize(vec2(_logicalSize));
    _shaders->thickLineShader.setColor(vec3(0.5));
    _gridFloor->draw();

    _shaders->solidShader.bind();
    _shaders->solidShader.setDiffuse(vec3(1, 0, 0));
    _shaders->solidShader.setAmbient(vec3(0.5, 0, 0));
    _shaders->solidShader.setMVPMatrix(_camera.matrix(), MVP);

    for (auto& [item, renderer] : _meshRenderers) {
        renderer->drawFaces();
    }

    _shaders->thickLineShader.bind();
    _shaders->thickLineShader.setWidth(1.f);
    _shaders->thickLineShader.setColor(vec3(0));
    for (auto& [item, renderer] : _meshRenderers) {
        renderer->drawEdges();
    }

    _shaders->circleShader.bind();
    _shaders->circleShader.setMVPMatrix(MVP);
    _shaders->circleShader.setViewportSize(vec2(_logicalSize));
    _shaders->circleShader.setWidth(5.f);
    _shaders->circleShader.setColor(vec3(1));
    for (auto& [item, renderer] : _meshRenderers) {
        renderer->drawVertices();
    }
}

} // namespace Lattice
