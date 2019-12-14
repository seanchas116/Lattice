#include "KeyObserver.hpp"
#include <QApplication>
#include <QKeyEvent>
#include <QWindow>
#include <QtDebug>

namespace Lattice {
namespace Editor {

KeyObserver::KeyObserver() {
    qApp->installEventFilter(this);
}

KeyObserver::~KeyObserver() {
    qApp->removeEventFilter(this);
}

bool KeyObserver::eventFilter(QObject *watched, QEvent *event) {
    if (!qobject_cast<QWindow *>(watched)) {
        return false;
    }
    switch (event->type()) {
    case QEvent::KeyPress:
        keyPress(static_cast<QKeyEvent *>(event));
        break;
    case QEvent::KeyRelease:
        keyRelease(static_cast<QKeyEvent *>(event));
        break;
    default:
        break;
    }
    return false;
}

void KeyObserver::keyPress(QKeyEvent *event) {
    _pressedKeys.insert(event->key());
    emit pressedKeysChanged(_pressedKeys);
}

void KeyObserver::keyRelease(QKeyEvent *event) {
    _pressedKeys.erase(event->key());
    emit pressedKeysChanged(_pressedKeys);
}

} // namespace Editor
} // namespace Lattice
