#pragma once
#include <QObject>
#include <unordered_set>

class QKeyEvent;

namespace Lattice::Editor {

class KeyObserver final : public QObject {
    Q_OBJECT
public:
    KeyObserver();
    ~KeyObserver() override;

    bool eventFilter(QObject *watched, QEvent *event) override;
    void keyPress(QKeyEvent* event);
    void keyRelease(QKeyEvent* event);

    const auto& pressedKeys() const { return _pressedKeys; }

signals:
    void selectedKeysChanged(const std::unordered_set<int>& keys);

private:
    std::unordered_set<int> _pressedKeys;
};

} // namespace Lattice
