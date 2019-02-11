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

struct HitResult {
    double depth;
    Opt<SP<Document::MeshVertex>> vertex;
    Opt<SP<Document::MeshEdge>> edge;
    Opt<SP<Document::MeshFace>> face;
};

struct MouseEvent {
public:
    MouseEvent(QMouseEvent* originalEvent, glm::dvec2 screenPos, const SP<Camera>& camera, const HitResult& hitResult) :
        originalEvent(originalEvent),
        screenPos(screenPos),
        camera(camera),
        hitResult(hitResult)
    {}

    glm::dvec3 worldPos() const;

    QMouseEvent* originalEvent;
    glm::dvec2 screenPos;
    const SP<Camera>& camera;
    HitResult hitResult;
};

} // namespace Render
} // namespace Lattice
