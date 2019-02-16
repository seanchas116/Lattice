#pragma once

#include <glm/vec2.hpp>
#include <QOpenGLExtraFunctions>
#include "../support/Shorthands.hpp"
#include "../support/PixelData.hpp"

namespace Lattice::GL {

class Texture;

class Framebuffer final : protected QOpenGLExtraFunctions {
    Q_DISABLE_COPY(Framebuffer)
public:
    Framebuffer(glm::ivec2 size); // default framebuffer
    Framebuffer(glm::ivec2 size, const std::vector<SP<Texture>>& colorBuffers, const Opt<SP<Texture>>& depthStencilBuffer = {});
    ~Framebuffer();

    void bind();
    void unbind();

    void readPixels(glm::ivec2 pos, PixelData<glm::u8vec4>& imageData);
    void readPixels(glm::ivec2 pos, PixelData<glm::vec4>& imageData);

    void readDepths(glm::ivec2 pos, PixelData<float>& imageData);
    float readDepth(glm::ivec2 pos);

    void blitTo(Framebuffer& dst, GLbitfield buffers = GL_COLOR_BUFFER_BIT, GLenum filter = GL_NEAREST);

private:
    bool _isDefault = false;
    GLuint _name = 0;
    glm::ivec2 _size;
    std::vector<SP<Texture>> _colorBuffers;
    Opt<SP<Texture>> _depthStencilBuffer;
};

} // namespace Lattice
