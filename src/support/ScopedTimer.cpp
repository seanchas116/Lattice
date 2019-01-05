#include "ScopedTimer.hpp"
#include <QtDebug>

namespace Lattice {

ScopedTimer::ScopedTimer(const std::string &title) : _title(title) {
    _timer.start();
}

ScopedTimer::~ScopedTimer() {
    qDebug() << _title.c_str() << _timer.elapsed() << "ms";
}

} // namespace Lattice
