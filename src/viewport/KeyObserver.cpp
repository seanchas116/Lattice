#include "KeyObserver.hpp"
#include <QKeyEvent>

namespace Lattice::Viewport {

KeyObserver::KeyObserver() {
}

void KeyObserver::keyPress(QKeyEvent *event) {
    _pressedKeys.insert(event->key());
    emit selectedKeysChanged(_pressedKeys);
}

void KeyObserver::keyRelease(QKeyEvent *event) {
    _pressedKeys.erase(event->key());
    emit selectedKeysChanged(_pressedKeys);
}

} // namespace Lattice
