#pragma once

class QOpenGLContext;
class QSurface;

namespace Lattice {
namespace GL {

class ContextRecallable {
public:
    ContextRecallable();

    void recallContext();

private:
    QOpenGLContext* _context;
    QSurface* _surface;
};

}
} // namespace Lattice
