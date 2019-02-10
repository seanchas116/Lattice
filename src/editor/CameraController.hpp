#pragma once

#include <QObject>
#include <QPoint>
#include <unordered_set>
#include "../support/Location.hpp"
#include "../support/Camera.hpp"

class QMouseEvent;
class QWheelEvent;

namespace Lattice::Editor {

class CameraController final : public QObject {
    Q_OBJECT
public:
    CameraController(const SP<Camera>& camera, QWidget* widget);
    bool mousePress(QMouseEvent* event);
    bool mouseMove(QMouseEvent* event);
    bool mouseRelease(QMouseEvent* event);
    bool wheel(QWheelEvent* event);

    void setPressedKeys(const std::unordered_set<int>& keys);

private:
    enum Mode {
        None,
        Move,
        Rotate,
    };

    SP<Camera> _camera;
    QWidget *_widget;

    Mode _mode;
    QPoint _lastMousePos;
    std::unordered_set<int> _pressedKeys;
    bool _rotateKey = false;
    bool _moveKey = false;
    bool _isOverridingCursor = false;
};

} // namespace Lattice
