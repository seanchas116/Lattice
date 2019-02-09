#pragma once
#include <QOpenGLExtraFunctions>
#include <array>

namespace Lattice {
namespace GL {

class IndexBuffer final : protected QOpenGLExtraFunctions {
    Q_DISABLE_COPY(IndexBuffer)
public:
    using Triangle = std::array<uint32_t, 3>;
    using LineStrip = std::vector<uint32_t>;
    using Line = std::array<uint32_t, 2>;

    IndexBuffer();
    ~IndexBuffer();

    void bind();
    void unbind();

    void setTriangles(const std::vector<Triangle>& triangles);
    void setLines(const std::vector<Line>& lines);
    void setLineStrips(const std::vector<LineStrip> &strips);

    size_t size() const { return _size; }

private:
    GLuint _buffer = 0;
    size_t _size = 0;
};

} // namespace GL
} // namespace Lattice
