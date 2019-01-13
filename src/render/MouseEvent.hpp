#pragma once
#include <QMouseEvent>
#include "../support/Camera.hpp"

namespace Lattice {
namespace Render {

struct HitResult {
    double t;
};

struct MouseEvent {
public:
    MouseEvent(QMouseEvent* originalEvent, glm::dvec2 screenPos, const Camera& camera, const HitResult& hitResult) :
        originalEvent(originalEvent),
        screenPos(screenPos),
        camera(camera),
        hitResult(hitResult)
    {}

    QMouseEvent* originalEvent;
    glm::dvec2 screenPos;
    const Camera& camera;
    HitResult hitResult;
};

} // namespace Render
} // namespace Lattice
