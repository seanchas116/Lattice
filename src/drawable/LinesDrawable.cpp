#include "LinesDrawable.hpp"
#include "LinesGeometry.hpp"
#include "../support/Camera.hpp"
#include "../resource/Resource.hpp"
#include "../gl/Shader.hpp"
#include "../gl/VAO.hpp"
#include "../gl/IndexBuffer.hpp"
#include "../gl/VertexBuffer.hpp"
#include <glm/gtx/transform.hpp>

namespace Lattice {
namespace Drawable {

namespace {

SP<GL::Shader> createShader() {
    return makeShared<GL::Shader>(
        Resource::read("src/drawable/LinesDrawable.vert"),
        Resource::read("src/drawable/LinesDrawable.geom"),
        Resource::read("src/drawable/LinesDrawable.frag"));
}

}

LinesDrawable::LinesDrawable() : _geometry(makeShared<LinesGeometry>()) {
}

void LinesDrawable::draw(SingletonBag& singletonBag, const glm::dmat4 &matrix, const SP<Camera> &camera) {
    auto shader = singletonBag.get(createShader);

    shader->bind();
    shader->setUniform("MV", camera->worldToCameraMatrix() * matrix);
    shader->setUniform("P", camera->cameraToViewportMatrix());
    shader->setUniform("viewportSize", camera->viewportSize());
    shader->setUniform("zNear", camera->projection() == Camera::Projection::Perspective ? camera->zNear() : -10000.0); // TODO: specify depth in better way
    shader->setUniform("width", _width);
    shader->setUniform("color", _color);
    shader->setUniform("useVertexColor", _useVertexColor);
    shader->setUniform("zOffset", _zOffset);
    _geometry->_vao->draw();
}

} // namespace Drawable
} // namespace Lattice
