#include "HitAreaMap.hpp"
#include "Renderable.hpp"
#include "../gl/Framebuffer.hpp"
#include "../gl/Texture.hpp"
#include "../gl/Binder.hpp"
#include "../support/Debug.hpp"
#include "../support/Camera.hpp"
#include <glm/gtc/type_precision.hpp>

using namespace glm;

namespace Lattice {
namespace Viewport {

HitAreaMap::HitAreaMap() : _framebuffer(makeShared<GL::Framebuffer>(glm::ivec2(0, 0))),
                           _additionalInfoFramebuffer(makeShared<GL::Framebuffer>(glm::ivec2(0, 0))) {
    initializeOpenGLFunctions();
}

void HitAreaMap::resize(glm::ivec2 size) {
    if (size == _framebufferSize) {
        return;
    }
    _framebuffer = makeShared<GL::Framebuffer>(size,
                                               std::vector{makeShared<GL::Texture>(size, GL::Texture::Format::RGBA32F)},
                                               makeShared<GL::Texture>(size, GL::Texture::Format::Depth24Stencil8));
    _additionalInfoFramebuffer = makeShared<GL::Framebuffer>(size,
                                                              std::vector{makeShared<GL::Texture>(size, GL::Texture::Format::RGBA32F)},
                                                             makeShared<GL::Texture>(size, GL::Texture::Format::Depth24Stencil8));

    _framebufferSize = size;
}

Opt<std::pair<SP<Renderable>, double>> HitAreaMap::pick(vec2 physicalPos) {
    recallContext();
    PixelData<vec4> pixels(glm::ivec2(1));
    _framebuffer->readPixels(physicalPos, pixels);
    auto renderable = Renderable::fromIDColor(pixels.data()[0]);

    if (!renderable) {
        return {};
    }
    float depth = _framebuffer->readDepth(physicalPos);
    return {{*renderable, depth}};
}

void HitAreaMap::draw(const SP<Renderable> &renderable, const SP<Draw::Operations> &operations, const SP<Camera> &camera) {
    resize(camera->viewportSize());

    GL::Binder binder(*_framebuffer);
    glClearColor(0, 0, 0, 0);
    glClearDepthf(1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderable->drawHitAreaRecursive(operations, camera);
    _lastRenderables.clear();
    renderable->getDescendants(_lastRenderables);
}

}
}
