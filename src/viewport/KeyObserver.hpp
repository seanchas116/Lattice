#pragma once
#include <QObject>
#include <unordered_set>

class QKeyEvent;

namespace Lattice {

class KeyObserver : public QObject {
    Q_OBJECT
public:
    KeyObserver();

    void keyPress(QKeyEvent* event);
    void keyRelease(QKeyEvent* event);

signals:
    void keysChanged(const std::unordered_set<int>& keys);

private:
    std::unordered_set<int> _keys;
};

} // namespace Lattice
