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

Opt<HitResult> HitAreaMap::pick(vec2 physicalPos) {
    recallContext();
    PixelData<vec4> pixels(glm::ivec2(1));
    _framebuffer->readPixels(physicalPos, pixels);

    PixelData<vec4> additionalInfoPixels(glm::ivec2(1));
    _additionalInfoFramebuffer->readPixels(physicalPos, additionalInfoPixels);

    auto renderable = Renderable::fromIDColor(pixels.data()[0]);

    if (!renderable) {
        return {};
    }
    float depth = _framebuffer->readDepth(physicalPos);
    return {{*renderable, depth, additionalInfoPixels.data()[0]}};
}

void HitAreaMap::draw(const SP<Renderable> &renderable, const SP<Draw::Operations> &operations, const SP<Camera> &camera) {
    resize(camera->viewportSize());

    {
        GL::Binder binder(*_framebuffer);
        operations->clear.clear(glm::vec4(0), 1);
        renderable->drawHitAreaRecursive(operations, camera);
    }

    {
        GL::Binder binder(*_additionalInfoFramebuffer);
        operations->clear.clear(glm::vec4(0), 1);
        renderable->drawHitUserColorRecursive(operations, camera);
    }

    _lastRenderables.clear();
    renderable->getDescendants(_lastRenderables);
}

}
}
