#include "ContextRecallable.hpp"
#include <QOpenGLContext>

namespace Lattice {
namespace GL {

ContextRecallable::ContextRecallable() {
    _context = QOpenGLContext::currentContext();
    _surface = _context->surface();
}

void ContextRecallable::recallContext() {
    _context->makeCurrent(_surface);
}

} // namespace GL
} // namespace Lattice
