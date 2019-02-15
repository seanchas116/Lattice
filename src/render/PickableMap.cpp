#include "PickableMap.hpp"
#include "RenderableObject.hpp"
#include "../gl/Framebuffer.hpp"
#include "../gl/Texture.hpp"
#include "../gl/Binder.hpp"
#include "../support/Debug.hpp"
#include "../support/Camera.hpp"
#include <glm/gtc/type_precision.hpp>

using namespace glm;

namespace Lattice::Render {

PickableMap::PickableMap() : _framebuffer(makeShared<GL::Framebuffer>(glm::ivec2(0, 0))) {
    initializeOpenGLFunctions();
}

void PickableMap::resize(glm::ivec2 size) {
    if (size == _framebufferSize) {
        return;
    }
    auto texture = makeShared<GL::Texture>(size, GL::Texture::Format::RGBA32F);
    auto depthTexture = makeShared<GL::Texture>(size, GL::Texture::Format::Depth24Stencil8);
    _framebuffer = makeShared<GL::Framebuffer>(size, std::vector{texture}, depthTexture);
    _framebufferSize = size;
}

Opt<SP<Renderable>> PickableMap::pick(vec2 physicalPos) {
    recallContext();

    PixelData<vec4> pixels(glm::ivec2(1));
    _framebuffer->readPixels(physicalPos + 0.5f, pixels);
    return Renderable::fromIDColor(pixels.data()[0]);
}

void PickableMap::draw(const std::vector<SP<RenderableObject>> &renderables, const SP<Operations> &operations, const SP<Camera> &camera) {
    resize(camera->viewSize());

    GL::Binder binder(*_framebuffer);
    glClearColor(0, 0, 0, 0);
    glClearDepthf(1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto& renderable : renderables) {
        renderable->drawPickables(operations, camera);
    }
}

}
