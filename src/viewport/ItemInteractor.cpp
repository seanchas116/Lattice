#include "ItemInteractor.hpp"
#include "ItemPicker.hpp"
#include "../support/Camera.hpp"
#include "../document/Item.hpp"
#include <QtDebug>

namespace Lattice {
namespace Viewport {

ItemInteractor::ItemInteractor(const SP<ItemPicker> &picker) : _picker(picker) {
}

bool ItemInteractor::mousePress(QMouseEvent *event, glm::dvec2 pos, const Camera &camera) {
    Q_UNUSED(event);
    auto [item, t] = _picker->pick(camera.worldMouseRay(pos));
    if (!item) {
        return true;
    }
    _draggedItem = item;
    qDebug() << item->name().c_str();
    return true;
}

bool ItemInteractor::mouseMove(QMouseEvent *event, glm::dvec2 pos, const Camera &camera) {
    Q_UNUSED(event); Q_UNUSED(pos); Q_UNUSED(camera);
    if (!_draggedItem) {
        return false;
    }
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
