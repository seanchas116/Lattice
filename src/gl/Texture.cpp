#include "Texture.hpp"

namespace Lattice::GL {

Texture::Texture(glm::ivec2 size, const void *pixels, GLint internalFormat, GLenum format, GLenum type) : _size(size) {
    initializeOpenGLFunctions();
    glGenTextures(1, &_name);
    glBindTexture(GL_TEXTURE_2D, _name);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, size.x, size.y, 0, format, type, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture() {
    glDeleteTextures(1, &_name);
}

void Texture::bind() {
    glBindTexture(GL_TEXTURE_2D, _name);
}

void Texture::unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

DepthStencilTexture::DepthStencilTexture(glm::ivec2 size, const void *pixels) {
    initializeOpenGLFunctions();
    glGenTextures(1, &_name);
    glBindTexture(GL_TEXTURE_2D, _name);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, size.x, size.y, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
}

DepthStencilTexture::~DepthStencilTexture() {
    glDeleteTextures(1, &_name);
}

void DepthStencilTexture::bind() {
    glBindTexture(GL_TEXTURE_2D, _name);
}

void DepthStencilTexture::unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

} // namespace Lattice
