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

signals:
    void onDragStart();
    void onDrag(glm::vec3 offset);
    void onDragEnd();

private:
    std::pair<glm::mat4, bool> manipulatorToWorldMatrix(glm::vec3 targetPosition, const Camera& camera) const;

    SP<VAO> _headVAO;
    SP<LineVAO> _bodyVAO;

    bool _isDragging = false;
    int _dragAxis = 0;
    float _originalValue = 0;
};

} // namespace Lattice
