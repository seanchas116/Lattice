#include "Framebuffer.hpp"
#include "Texture.hpp"
#include <QOpenGLContext>

namespace Lattice::OldGL {

Framebuffer::Framebuffer(glm::ivec2 size) :
    _isDefault(true),
    _size(size)
{
    initializeOpenGLFunctions();
    _name = QOpenGLContext::currentContext()->defaultFramebufferObject();
}

Framebuffer::Framebuffer(glm::ivec2 size, const std::vector<SP<Texture> > &colorBuffers, const std::optional<SP<DepthStencilTexture> > &depthStencilBuffer) :
    _size(size),
    _colorBuffers(colorBuffers),
    _depthStencilBuffer(depthStencilBuffer)
{
    initializeOpenGLFunctions();
    glGenFramebuffers(1, &_name);
    glBindFramebuffer(GL_FRAMEBUFFER, _name);

    for (size_t i = 0; i < colorBuffers.size(); ++i) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + uint32_t(i), GL_TEXTURE_2D,
                               colorBuffers[i]->name(), 0);
    }
    LATTICE_OPTIONAL_LET(depthStencil, depthStencilBuffer, {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthStencil->name(), 0);
    })

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::~Framebuffer() {
    if (!_isDefault) {
        glDeleteFramebuffers(1, &_name);
    }
}

void Framebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, _name);

    if (!_isDefault) {
        std::vector<GLenum> targets;
        for (size_t i = 0; i < _colorBuffers.size(); ++i) {
            targets.push_back(GL_COLOR_ATTACHMENT0 + uint32_t(i));
        }
        glDrawBuffers(int(targets.size()), targets.data());
    } else {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        GLenum target = GL_COLOR_ATTACHMENT0;
        glDrawBuffers(1, &target);
    }

    glViewport(0, 0, _size.x, _size.y);
}

void Framebuffer::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::readPixels(glm::ivec2 pos, PixelData<glm::u8vec4> &imageData) {
    glBindFramebuffer(GL_FRAMEBUFFER, _name);
    glReadPixels(pos.x, pos.y, imageData.size().x, imageData.size().y,
                 GL_RGBA, GL_UNSIGNED_BYTE, imageData.data().data());
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::u8vec4 Framebuffer::readPixel(glm::ivec2 pos) {
    PixelData<glm::u8vec4> data({1, 1});
    readPixels(pos, data);
    return data.data()[0];
}

void Framebuffer::readDepths(glm::ivec2 pos, PixelData<float> &imageData) {
    glBindFramebuffer(GL_FRAMEBUFFER, _name);
    glReadPixels(pos.x, pos.y, imageData.size().x, imageData.size().y,
                 GL_DEPTH_COMPONENT, GL_FLOAT, imageData.data().data());
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

float Framebuffer::readDepth(glm::ivec2 pos) {
    PixelData<float> data({1, 1});
    readDepths(pos, data);
    return data.data()[0];
}

void Framebuffer::blitTo(Framebuffer &dst, GLbitfield buffers, GLenum filter) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, _name);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst._name);
    glBlitFramebuffer(0, 0, _size.x, _size.y, 0, 0, dst._size.x, dst._size.y, buffers, filter);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

} // namespace Lattice
