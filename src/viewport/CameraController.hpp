#pragma once

#include <QObject>
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
    Camera _camera;
};

} // namespace Lattice
