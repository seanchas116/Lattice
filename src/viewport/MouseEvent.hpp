#pragma once
#include <QMouseEvent>
#include "../support/OldCamera.hpp"
#include "../support/Shorthands.hpp"

namespace Lattice {
namespace Viewport {

struct MouseEvent {
    glm::dvec3 viewportPos;
    SP<OldCamera> camera;
    QMouseEvent* originalMouseEvent;
    QContextMenuEvent* originalContextMenuEvent;

    glm::dvec3 worldPos() const;
};

} // namespace Render
} // namespace Lattice
