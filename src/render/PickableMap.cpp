#include "PickableMap.hpp"
#include "../gl/Framebuffer.hpp"
#include "../gl/Texture.hpp"
#include "../support/Debug.hpp"
#include <glm/gtc/type_precision.hpp>

using namespace glm;

namespace Lattice::Render {

namespace {

/*
vec3 idToColor(uint32_t id) {
    uint32_t r = id & 0xFF;
    uint32_t g = id >> 8 & 0xFF;
    uint32_t b = id >> 16 & 0xFF;
    return vec3(r / 255.f, g / 255.f, b / 255.f);
}

uint32_t colorToID(u8vec4 color) {
    return uint32_t(color.r) | uint32_t(color.g) << 8 | uint32_t(color.b) << 16;
}
*/

}

PickableMap::PickableMap() : _framebuffer(makeShared<GL::Framebuffer>(glm::ivec2(0, 0))) {
    initializeOpenGLFunctions();
}

/*
void PickableMap::render(const MVP &context, const QVector<SP<Entity>> &entities) {
    _devicePixelRatio = vec2(_framebufferSize) / vec2(context.viewportSize());

    _framebuffer->bind();

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _entities.clear();
    for (auto& entity : entities) {
        renderEntity(context, entity);
    }
    _framebuffer->unbind();
}

SP<Pickable> PickableMap::pick(glm::vec2 screenPos) {
    if (!_framebuffer) {
        return {};
    }
    vec2 pos = round(screenPos * _devicePixelRatio);

    u8vec4 color = _framebuffer->readPixel(pos);
    uint32_t id = colorToID(color);
    int index = int(id) - 1;
    if (index < 0 || int(_entities.size()) <= index) {
        return {};
    }
    return _entities[index];
}
*/

void PickableMap::resize(glm::ivec2 size) {
    if (size == _framebufferSize) {
        return;
    }
    auto texture = makeShared<GL::Texture>(size);
    auto depthTexture = makeShared<GL::DepthStencilTexture>(size);
    _framebuffer = makeShared<GL::Framebuffer>(size, std::vector{texture}, depthTexture);
    _framebufferSize = size;
}

void PickableMap::draw(const std::vector<Renderable> &renderables, const SP<Operations> &operations, const SP<Camera> &camera) {
    Q_UNUSED(renderables);
    Q_UNUSED(operations);
    Q_UNUSED(camera);
    // TODO
}

/*
void PickableMap::renderEntity(const MVP& mvp, const SP<Entity> &entity) {
    // TODO: apply entity transform to context

    int i = _entities.size();
    vec3 color = idToColor(uint32_t(i + 1));
    entity->renderMouseArea(mvp, color);
    _entities << entity;

    for (const auto& child : entity->childEntities()) {
        renderEntity(mvp, child);
    }
}
*/

}
