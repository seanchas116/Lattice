#include "ViewportRenderer.hpp"
#include "GridFloor.hpp"
#include "MeshRenderer.hpp"
#include "CircleShader.hpp"
#include "ThickLineShader.hpp"
#include "SolidShader.hpp"
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

    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

    _circleShader = std::make_shared<CircleShader>();
    _circleShader->setWidth(4.f);

    _lineShader = std::make_shared<ThickLineShader>();
    _lineShader->setWidth(1.f);

    _solidShader = std::make_shared<SolidShader>();
    _solidShader->setDiffuse(vec3(1, 0, 0));

    _gridFloor = std::make_shared<GridFloor>();
}

void ViewportRenderer::resize(ivec2 physicalSize, ivec2 logicalSize) {
    glViewport(0, 0, physicalSize.x, physicalSize.y);
    _logicalSize = logicalSize;
    _projection = glm::perspective(glm::radians(60.f), float(physicalSize.x) / float(physicalSize.y), 0.1f, 100.f);
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

    auto MVP = _projection * _camera.matrix();

    _lineShader->bind();
    _lineShader->setMVPMatrix(MVP);
    _lineShader->setViewportSize(vec2(_logicalSize));

    _lineShader->setColor(vec3(0.5));
    _gridFloor->draw();

    _solidShader->bind();
    _solidShader->setMVPMatrix(_camera.matrix(), MVP);

    for (auto& [item, renderer] : _meshRenderers) {
        renderer->drawFaces();
    }

    _lineShader->bind();
    _lineShader->setColor(vec3(0));
    for (auto& [item, renderer] : _meshRenderers) {
        renderer->drawEdges();
    }

    _circleShader->bind();
    _circleShader->setMVPMatrix(MVP);
    _circleShader->setViewportSize(vec2(_logicalSize));
    _circleShader->setColor(vec3(1));
    for (auto& [item, renderer] : _meshRenderers) {
        renderer->drawVertices();
    }
}

} // namespace Lattice
