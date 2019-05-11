#pragma once
#include <QMouseEvent>
#include "../support/Camera.hpp"
#include "../support/Shorthands.hpp"

namespace Lattice {
namespace Viewport {

struct MouseEvent {
    glm::dvec3 viewportPos;
    glm::vec4 hitUserColor;
    SP<Camera> camera;
    QMouseEvent* originalMouseEvent;
    QContextMenuEvent* originalContextMenuEvent;

    glm::dvec3 worldPos() const;
};

} // namespace Render
} // namespace Lattice
