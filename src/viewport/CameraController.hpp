#pragma once

#include <QObject>
#include <QPoint>
#include "../support/Camera.hpp"

class QMouseEvent;

namespace Lattice {

class CameraController : public QObject {
    Q_OBJECT
public:
    CameraController();
    bool mousePress(QMouseEvent* event);
    bool mouseMove(QMouseEvent* event);
    bool mouseRelease(QMouseEvent* event);

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
};

} // namespace Lattice
