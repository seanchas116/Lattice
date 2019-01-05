#include "ItemInteractor.hpp"
#include "ItemPicker.hpp"
#include "../document/Item.hpp"
#include "../support/Camera.hpp"
#include "../support/Debug.hpp"
#include <QtDebug>

namespace Lattice {
namespace Viewport {

ItemInteractor::ItemInteractor(const SP<ItemPicker> &picker) : _picker(picker) {
}

bool ItemInteractor::mousePress(QMouseEvent *event, glm::dvec2 pos, const Camera &camera) {
    Q_UNUSED(event);
    auto worldMouseRay = camera.worldMouseRay(pos);
    auto [item, t] = _picker->pick(worldMouseRay);
    if (!item) {
        return false;
    }
    _draggedItem = item;

    glm::dvec3 worldDragPos = worldMouseRay.at(t);
    auto [screenDragPos, isInScreen] = camera.mapWorldToScreen(worldDragPos);
    if (!isInScreen) {
        return false;
    }
    auto worldDragPos2 = camera.mapScreenToWorld(screenDragPos);
    qDebug() << "initial pos" << worldDragPos << worldDragPos2;

    _initialLocation = _draggedItem->location();
    _initialWorldPos = worldDragPos;
    _initialDragDepth = screenDragPos.z;
    return true;
}

bool ItemInteractor::mouseMove(QMouseEvent *event, glm::dvec2 pos, const Camera &camera) {
    Q_UNUSED(event); Q_UNUSED(pos); Q_UNUSED(camera);
    if (!_draggedItem) {
        return false;
    }

    auto newWorldPos = camera.mapScreenToWorld(glm::dvec3(pos, _initialDragDepth));
    qDebug() << "drag pos" << newWorldPos;
    auto newLocation = _initialLocation;
    newLocation.position += newWorldPos - _initialWorldPos;
    _draggedItem->setLocation(newLocation);

    return true;
}

bool ItemInteractor::mouseRelease(QMouseEvent *event, glm::dvec2 pos, const Camera &camera) {
    Q_UNUSED(event); Q_UNUSED(pos); Q_UNUSED(camera);
    if (!_draggedItem) {
        return false;
    }
    _draggedItem = {};
    return true;
}

} // namespace Viewport
} // namespace Lattice
