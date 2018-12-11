#include "ContextRecallable.hpp"
#include <QOpenGLContext>

namespace Lattice {

ContextRecallable::ContextRecallable() {
    _context = QOpenGLContext::currentContext();
    _surface = _context->surface();
}

void ContextRecallable::recallContext() {
    _context->makeCurrent(_surface);
}

} // namespace Lattice
