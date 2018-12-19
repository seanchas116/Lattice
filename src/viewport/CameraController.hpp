#pragma once

#include <QObject>
#include <QPoint>
#include <unordered_set>
#include "../support/Camera.hpp"

class QMouseEvent;
class QWheelEvent;

namespace Lattice {

class CameraController : public QObject {
    Q_OBJECT
public:
    CameraController();
    bool mousePress(QMouseEvent* event);
    bool mouseMove(QMouseEvent* event);
    bool mouseRelease(QMouseEvent* event);
    bool wheel(QWheelEvent* event);

    void setPressedKeys(const std::unordered_set<int>& keys);

signals:
    void cameraChanged(const Camera& camera);

private:
    enum Mode {
        None,
        Move,
        Rotate,
    };

    Camera _camera;
    Mode _mode;
    QPoint _lastMousePos;
    std::unordered_set<int> _pressedKeys;
};

} // namespace Lattice
