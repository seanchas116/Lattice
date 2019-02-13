#pragma once

#include <glm/vec2.hpp>
#include <QOpenGLExtraFunctions>

namespace Lattice::GL {

class Texture final : protected QOpenGLExtraFunctions {
    Q_DISABLE_COPY(Texture)
public:
    Texture(glm::ivec2 size, const void* pixels = nullptr, GLint internalFormat = GL_RGBA8, GLenum format = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE);
    ~Texture();

    glm::ivec2 size() const { return _size; }
    GLuint name() const { return _name; }

    void bind();
    void unbind();

private:
    GLuint _name = 0;
    glm::ivec2 _size;
};

class DepthStencilTexture final : protected QOpenGLExtraFunctions {
    Q_DISABLE_COPY(DepthStencilTexture)
public:
    DepthStencilTexture(glm::ivec2 size, const void* pixels = nullptr);
    ~DepthStencilTexture();

    glm::ivec2 size() const { return _size; }
    GLuint name() const { return _name; }

    void bind();
    void unbind();

private:
    GLuint _name = 0;
    glm::ivec2 _size;
};


} // namespace Lattice
