#pragma once
#include <QOpenGLExtraFunctions>

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

} // namespace GL
} // namespace Lattice
