#pragma once

#include <glm/glm.hpp>
#include "Operations.hpp"
#include "../support/Camera.hpp"

class QMouseEvent;

namespace Lattice {

class Manipulator {
public:
    Manipulator();

    void draw(const SP<Operations>& operations, const Camera& camera);

    bool mousePress(QMouseEvent* event, glm::vec2 pos, const Camera& camera);
    bool mouseMove(QMouseEvent* event, glm::vec2 pos, const Camera& camera);
    bool mouseRelease(QMouseEvent* event, glm::vec2 pos, const Camera& camera);

    void setTargetPosition(glm::vec3 pos) { _targetPosition = pos; }

signals:
    void onDragStart();
    void onDrag(glm::vec3 offset);
    void onDragEnd();

private:
    std::pair<glm::mat4, bool> manipulatorToWorldMatrix(const Camera& camera) const;
    std::tuple<float, float, bool> distanceFromArrow(glm::vec2 screenPos, const Camera& camera);

    SP<VAO> _headVAO;
    SP<LineVAO> _bodyVAO;

    glm::vec3 _targetPosition {0};

    bool _isDragging = false;
    int _dragAxis = 0;
    float _initialDragValue = 0;
};

} // namespace Lattice
