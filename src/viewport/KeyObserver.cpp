#include "KeyObserver.hpp"
#include <QKeyEvent>

namespace Lattice {

KeyObserver::KeyObserver() {
}

void KeyObserver::keyPress(QKeyEvent *event) {
    _keys.insert(event->key());
    emit keysChanged(_keys);
}

void KeyObserver::keyRelease(QKeyEvent *event) {
    _keys.erase(event->key());
    emit keysChanged(_keys);
}

} // namespace Lattice
