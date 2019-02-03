#pragma once

#include <glm/vec2.hpp>
#include <QOpenGLExtraFunctions>

namespace Lattice::OldGL {

class Texture final : protected QOpenGLExtraFunctions {
    Q_DISABLE_COPY(Texture)
public:
    Texture(glm::ivec2 size, const void* pixels = nullptr);
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
