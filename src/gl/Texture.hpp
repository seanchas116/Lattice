#pragma once

#include <QOpenGLExtraFunctions>
#include <glm/vec2.hpp>

namespace Lattice {
namespace GL {

class Texture final : protected QOpenGLExtraFunctions {
    Q_DISABLE_COPY(Texture)
  public:
    enum class Format {
        RGBA8,
        RGBA32F,
        Depth24Stencil8
    };

    Texture(glm::ivec2 size, Format format = Format::RGBA8, const void *pixels = nullptr);
    ~Texture();

    glm::ivec2 size() const { return _size; }
    GLuint name() const { return _name; }

    void bind();
    void unbind();

  private:
    GLuint _name = 0;
    glm::ivec2 _size;
};

} // namespace GL
} // namespace Lattice
