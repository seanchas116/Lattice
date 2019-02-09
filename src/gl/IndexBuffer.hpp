#pragma once
#include <QOpenGLExtraFunctions>
#include <array>

namespace Lattice {
namespace GL {

class IndexBuffer : protected QOpenGLExtraFunctions {
    Q_DISABLE_COPY(IndexBuffer)
public:
    IndexBuffer();
    virtual ~IndexBuffer();

    void bind();
    void unbind();
    virtual void draw() = 0;
protected:
    GLuint _buffer = 0;
};

class TriangleIndexBuffer : public IndexBuffer {
public:
    using Triangle = std::array<uint32_t, 3>;

    size_t size() const { return _size; }
    void setTriangles(const std::vector<Triangle>& triangles);

private:
    size_t _size;
};

} // namespace GL
} // namespace Lattice
