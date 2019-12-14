#include "HitAreaMap.hpp"
#include "../gl/Binder.hpp"
#include "../gl/Framebuffer.hpp"
#include "../gl/Texture.hpp"
#include "../support/Camera.hpp"
#include "../support/Debug.hpp"
#include "Renderable.hpp"
#include <glm/gtc/type_precision.hpp>

using namespace glm;

namespace Lattice {
namespace Viewport {

HitAreaMap::HitAreaMap() : _framebuffer(makeShared<GL::Framebuffer>(glm::ivec2(0, 0))) {
}

void HitAreaMap::resize(glm::ivec2 size) {
    if (size == _framebufferSize) {
        return;
    }
    _framebuffer = makeShared<GL::Framebuffer>(size,
                                               std::vector{makeShared<GL::Texture>(size, GL::Texture::Format::RGBA32F)},
                                               makeShared<GL::Texture>(size, GL::Texture::Format::Depth24Stencil8));

    _framebufferSize = size;
}

Opt<HitResult> HitAreaMap::pick(vec2 physicalPos) {
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

void HitAreaMap::draw(const SP<Renderable> &renderable, const DrawEvent &drawEvent) {
    resize(drawEvent.camera.viewportSize());

    {
        GL::Binder binder(*_framebuffer);
        drawEvent.operations->clear.clear(glm::vec4(0), 1);
        renderable->drawHitAreaRecursive(drawEvent);
    }

    _lastRenderables.clear();
    renderable->getDescendants(_lastRenderables);
}

} // namespace Viewport
} // namespace Lattice
