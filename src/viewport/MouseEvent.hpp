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

namespace Viewport {

struct LocatedEvent {
    Q_DISABLE_COPY(LocatedEvent)
public:
    LocatedEvent(glm::dvec3 viewportPos, glm::vec4 hitUserColor, const SP<Camera>& camera) :
        viewportPos(viewportPos),
        hitUserColor(hitUserColor),
        camera(camera)
    {}
    virtual ~LocatedEvent();

    glm::dvec3 worldPos() const;

    glm::dvec3 viewportPos;
    glm::vec4 hitUserColor;
    const SP<Camera>& camera;
};

struct MouseEvent : public LocatedEvent {
public:
    MouseEvent(QMouseEvent* originalEvent, glm::dvec3 viewportPos, glm::vec4 hitUserColor, const SP<Camera>& camera) :
        LocatedEvent(viewportPos, hitUserColor, camera),
        originalEvent(originalEvent)
    {}

    QMouseEvent* originalEvent;
};

struct ContextMenuEvent : public LocatedEvent {
public:
    ContextMenuEvent(QContextMenuEvent* originalEvent, glm::dvec3 viewportPos, glm::vec4 hitUserColor, const SP<Camera>& camera) :
        LocatedEvent(viewportPos, hitUserColor, camera),
        originalEvent(originalEvent)
    {}

    QContextMenuEvent* originalEvent;
};


} // namespace Render
} // namespace Lattice
