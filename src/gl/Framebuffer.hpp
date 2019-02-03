#pragma once

#include <glm/vec2.hpp>
#include <QOpenGLExtraFunctions>
#include "../support/Pointer.hpp"
#include "../support/PixelData.hpp"

namespace Lattice::OldGL {

class Texture;
class DepthStencilTexture;

class Framebuffer final : protected QOpenGLExtraFunctions {
    Q_DISABLE_COPY(Framebuffer)
public:
    Framebuffer(glm::ivec2 size); // default framebuffer
    Framebuffer(glm::ivec2 size, const std::vector<SP<Texture>>& colorBuffers, const std::optional<SP<DepthStencilTexture>>& depthStencilBuffer = {});
    ~Framebuffer();

    void bind();
    void unbind();

    void readPixels(glm::ivec2 pos, PixelData<glm::u8vec4>& imageData);
    glm::u8vec4 readPixel(glm::ivec2 pos);

    void readDepths(glm::ivec2 pos, PixelData<float>& imageData);
    float readDepth(glm::ivec2 pos);

    void blitTo(Framebuffer& dst, GLbitfield buffers = GL_COLOR_BUFFER_BIT, GLenum filter = GL_NEAREST);

private:
    bool _isDefault = false;
    GLuint _name = 0;
    glm::ivec2 _size;
    std::vector<SP<Texture>> _colorBuffers;
    std::optional<SP<DepthStencilTexture>> _depthStencilBuffer;
};

} // namespace Lattice
