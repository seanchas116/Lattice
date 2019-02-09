#pragma once
#include <QOpenGLExtraFunctions>
#include "../support/Pointer.hpp"

namespace Lattice {
namespace GL {

class IndexBuffer;

class DrawMethod : public QOpenGLExtraFunctions {
    Q_DISABLE_COPY(DrawMethod)
public:
    DrawMethod();
    virtual ~DrawMethod();
    virtual void bind();
    virtual void unbind();
    virtual void draw() = 0;
};

class PointsDrawMethod : public DrawMethod {
public:
    size_t first() const { return _first; }
    void setFirst(size_t first) { _first = first; }
    size_t count() const { return _count; }
    void setCount(size_t count) { _count = count; }

    void draw() override;

private:
    size_t _first = 0;
    size_t _count = 0;
};

} // namespace GL
} // namespace Lattice
