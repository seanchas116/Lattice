#include "ViewportRenderer.hpp"
#include "GridFloor.hpp"
#include "MeshRenderer.hpp"
#include "../gl/Shader.hpp"
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
    _lineShader = std::make_shared<Shader>(readResource("src/viewport/ThickLine.vert"), readResource("src/viewport/ThickLine.geom"), readResource("src/viewport/ThickLine.frag"));
    _solidShader = std::make_shared<Shader>(readResource("src/viewport/Solid.vert"), std::string(), readResource("src/viewport/Solid.frag"));
    _gridFloor = std::make_shared<GridFloor>();
}

void ViewportRenderer::resize(glm::ivec2 size) {
    glViewport(0, 0, size.x, size.y);
    _size = size;
    _projection = glm::perspective(glm::radians(60.f), float(size.x) / float(size.y), 0.1f, 100.f);
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

    _lineShader->bind();
    _lineShader->setUniform("zOffset", 0.f);
    _lineShader->setUniform("MVP", _projection * _camera.matrix());
    _lineShader->setUniform("width", 1.f);
    _lineShader->setUniform("color", vec3(0, 0, 0));
    _lineShader->setUniform("viewportSize", vec2(_size));
    _gridFloor->draw();
}

} // namespace Lattice
