#pragma once
#include <QMouseEvent>
#include "../support/Camera.hpp"
#include "../support/Shorthands.hpp"

namespace Lattice {

namespace Document {
class MeshVertex;
class MeshEdge;
class MeshFace;
}

namespace Render {

struct MouseEvent {
public:
    MouseEvent(QMouseEvent* originalEvent, glm::dvec2 screenPos, const SP<Camera>& camera, double depth) :
        originalEvent(originalEvent),
        screenPos(screenPos),
        camera(camera),
        depth(depth)
    {}

    glm::dvec3 worldPos() const;

    QMouseEvent* originalEvent;
    glm::dvec2 screenPos;
    const SP<Camera>& camera;
    double depth;
};

} // namespace Render
} // namespace Lattice
