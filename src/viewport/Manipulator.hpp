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

private:
    std::pair<glm::mat4, bool> manipulatorToWorldMatrix(glm::vec3 targetPosition, const Camera& camera) const;

    SP<VAO> _headVAO;
    SP<LineVAO> _bodyVAO;
};

} // namespace Lattice
