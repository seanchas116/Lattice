#pragma once
#include <QObject>
#include <unordered_set>

class QKeyEvent;

namespace Lattice {

class KeyObserver final : public QObject {
    Q_OBJECT
public:
    KeyObserver();

    void keyPress(QKeyEvent* event);
    void keyRelease(QKeyEvent* event);

    const auto& pressedKeys() const { return _pressedKeys; }

signals:
    void selectedKeysChanged(const std::unordered_set<int>& keys);

private:
    std::unordered_set<int> _pressedKeys;
};

} // namespace Lattice
