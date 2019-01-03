#pragma once

#include <glm/glm.hpp>
#include <QObject>
#include "Operations.hpp"
#include "../support/Camera.hpp"

class QMouseEvent;

namespace Lattice::Viewport {

class Manipulator final : public QObject {
    Q_OBJECT
public:
    Manipulator();

    void draw(const SP<Operations>& operations, const Camera& camera);

    bool mousePress(QMouseEvent* event, glm::dvec2 pos, const Camera& camera);
    bool mouseMove(QMouseEvent* event, glm::dvec2 pos, const Camera& camera);
    bool mouseRelease(QMouseEvent* event, glm::dvec2 pos, const Camera& camera);

    void setTargetPosition(glm::dvec3 pos) { _targetPosition = pos; }

signals:
    void onDragStart();
    void onDrag(glm::dvec3 offset);
    void onDragEnd();

private:
    SP<VAO> _headVAO;
    SP<LineVAO> _bodyVAO;

    glm::dvec3 _targetPosition {0};

    bool _isDragging = false;
    int _dragAxis = 0;
    glm::dvec3 _initialDragValue {0};
    glm::dvec3 _initialTargetPosition {0};
};

} // namespace Lattice
