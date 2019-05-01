#include "PickableMap.hpp"
#include "RenderableObject.hpp"
#include "../gl/Framebuffer.hpp"
#include "../gl/Texture.hpp"
#include "../gl/Binder.hpp"
#include "../support/Debug.hpp"
#include "../support/Camera.hpp"
#include <glm/gtc/type_precision.hpp>

using namespace glm;

namespace Lattice {
namespace Viewport {

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

Opt<std::pair<SP<Renderable>, double>> PickableMap::pick(vec2 physicalPos) {
    recallContext();
    PixelData<vec4> pixels(glm::ivec2(1));
    _framebuffer->readPixels(physicalPos, pixels);
    auto id = PickableID::fromIDColor(pixels.data()[0]);
    if (id.renderableID < 0 || int(_lastRenderables.size()) <= id.renderableID) {
        return {};
    }

    auto& renderable = _lastRenderables[id.renderableID];

    float depth = _framebuffer->readDepth(physicalPos);
    return {{renderable, depth}};
}

void PickableMap::draw(const SP<RenderableObject> &renderable, const SP<Draw::Operations> &operations, const SP<Camera> &camera) {
    resize(camera->viewportSize());

    GL::Binder binder(*_framebuffer);
    PickableID backgroundID(-1, 0);
    auto backgroundColor = backgroundID.toColor();

    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClearDepthf(1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _lastRenderables.clear();
    renderable->drawPickablesRecursive(operations, camera, _lastRenderables);
}

}
}
