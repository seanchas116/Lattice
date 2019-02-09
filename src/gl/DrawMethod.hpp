#pragma once
#include <QOpenGLExtraFunctions>

namespace Lattice {
namespace GL {

class DrawMethod : public QOpenGLExtraFunctions {
    Q_DISABLE_COPY(DrawMethod)
public:
    DrawMethod();
    virtual ~DrawMethod();
    virtual void draw() = 0;
};

} // namespace GL
} // namespace Lattice
