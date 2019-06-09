#pragma once

#include <QObject>
#include <QPoint>
#include <unordered_set>
#include "../support/Location.hpp"
#include "../support/OldCamera.hpp"

class QMouseEvent;
class QWheelEvent;

namespace Lattice {
namespace Editor {

class CameraController final : public QObject {
    Q_OBJECT
public:
    CameraController(const SP<OldCamera>& camera, QWidget* widget);
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

    Location location() const;

    SP<OldCamera> _camera;
    QWidget *_widget;

    Mode _mode;
    QPoint _lastMousePos;
    std::unordered_set<int> _pressedKeys;
    bool _rotateKey = false;
    bool _moveKey = false;
    bool _isOverridingCursor = false;
    glm::dvec3 _position {0, 10, 10};
    glm::dvec3 _eulerAngles {0, 0, 0};
};

}
} // namespace Lattice
