#include "Texture.hpp"

namespace Lattice {
namespace GL {

Texture::Texture(glm::ivec2 size, Texture::Format format, const void *pixels) {
    initializeOpenGLFunctions();

    GLint internalFormat;
    GLenum format_;
    GLenum type;

    switch (format) {
    case Format::RGBA8:
        internalFormat = GL_RGBA8;
        format_ = GL_RGBA;
        type = GL_UNSIGNED_BYTE;
        break;
    case Format::RGBA32F:
        internalFormat = GL_RGBA32F;
        format_ = GL_RGBA;
        type = GL_FLOAT;
        break;
    case Format::Depth24Stencil8:
        internalFormat = GL_DEPTH24_STENCIL8;
        format_ = GL_DEPTH_STENCIL;
        type = GL_UNSIGNED_INT_24_8;
        break;
    }

    glGenTextures(1, &_name);
    glBindTexture(GL_TEXTURE_2D, _name);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, size.x, size.y, 0, format_, type, pixels);
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

} // namespace GL
} // namespace Lattice
