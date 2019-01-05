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
    auto item = _picker->pick(camera.worldMouseRay(pos));
    qDebug() << item.get();
    return false;
}

bool ItemInteractor::mouseMove(QMouseEvent *event, glm::dvec2 pos, const Camera &camera) {
    Q_UNUSED(event); Q_UNUSED(pos); Q_UNUSED(camera);
    return false;
}

bool ItemInteractor::mouseRelease(QMouseEvent *event, glm::dvec2 pos, const Camera &camera) {
    Q_UNUSED(event); Q_UNUSED(pos); Q_UNUSED(camera);
    return false;
}

} // namespace Viewport
} // namespace Lattice
