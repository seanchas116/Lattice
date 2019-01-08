#include "ItemInteractor.hpp"
#include "ItemPicker.hpp"
#include "../document/Item.hpp"
#include "../document/Document.hpp"
#include "../document/History.hpp"
#include "../support/Camera.hpp"
#include "../support/Debug.hpp"
#include "../support/OptionalGuard.hpp"
#include <QtDebug>
#include <QMouseEvent>

namespace Lattice {
namespace Viewport {

ItemInteractor::ItemInteractor(const SP<ItemPicker> &picker) : _picker(picker) {
}

std::pair<bool, double> ItemInteractor::mousePress(QMouseEvent *event, glm::dvec2 pos, const Camera &camera) {
    auto worldMouseRay = camera.worldMouseRay(pos);
    LATTICE_OPTIONAL_GUARD(pickResult, _picker->pick(worldMouseRay), return {true, 0};)
    auto [item, t] = pickResult;
            LATTICE_OPTIONAL_GUARD(document, item->document(), return {true, 0};)
    _draggedItem = item;

    glm::dvec3 worldDragPos = worldMouseRay.at(t);
    auto [screenDragPos, isInScreen] = camera.mapWorldToScreen(worldDragPos);
    if (!isInScreen) {
        return {false, 0};
    }

    _initialLocation = item->location();
    _initialWorldPos = worldDragPos;
    _initialDragDepth = screenDragPos.z;
    _dragStarted = false;

    document->selectItem(item, event->modifiers() & Qt::ShiftModifier);

    return {true, 0};
}

void ItemInteractor::mouseMove(QMouseEvent *event, glm::dvec2 pos, const Camera &camera) {
    Q_UNUSED(event);
    LATTICE_OPTIONAL_GUARD(item, _draggedItem, return;)
    LATTICE_OPTIONAL_GUARD(document, item->document(), return;)

    auto newWorldPos = camera.mapScreenToWorld(glm::dvec3(pos, _initialDragDepth));
    auto newLocation = _initialLocation;
    newLocation.position += newWorldPos - _initialWorldPos;

    if (!_dragStarted) {
        document->history()->beginChange(tr("Move Item"));
        _dragStarted = true;
    }
    item->setLocation(newLocation);
}

void ItemInteractor::mouseRelease(QMouseEvent *event, glm::dvec2 pos, const Camera &camera) {
    Q_UNUSED(event); Q_UNUSED(pos); Q_UNUSED(camera);
    if (!_draggedItem) {
        return;
    }
    _draggedItem = {};
}

} // namespace Viewport
} // namespace Lattice
