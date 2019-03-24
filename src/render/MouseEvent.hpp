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

struct LocatedEvent {
    Q_DISABLE_COPY(LocatedEvent)
public:
    LocatedEvent(glm::dvec2 viewportPos, const SP<Camera>& camera, double depth) :
        viewportPos(viewportPos),
        camera(camera),
        depth(depth)
    {}
    virtual ~LocatedEvent();

    glm::dvec3 worldPos() const;

    glm::dvec2 viewportPos;
    const SP<Camera>& camera;
    double depth;
};

struct MouseEvent : public LocatedEvent {
public:
    MouseEvent(QMouseEvent* originalEvent, glm::dvec2 viewportPos, const SP<Camera>& camera, double depth) :
        LocatedEvent(viewportPos, camera, depth),
        originalEvent(originalEvent)
    {}

    QMouseEvent* originalEvent;
};

struct ContextMenuEvent : public LocatedEvent {
public:
    ContextMenuEvent(QContextMenuEvent* originalEvent, glm::dvec2 viewportPos, const SP<Camera>& camera, double depth) :
        LocatedEvent(viewportPos, camera, depth),
        originalEvent(originalEvent)
    {}

    QContextMenuEvent* originalEvent;
};


} // namespace Render
} // namespace Lattice
