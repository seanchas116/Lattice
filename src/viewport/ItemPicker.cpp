#include "ItemPicker.hpp"
#include "../document/Item.hpp"
#include "../document/MeshItem.hpp"

namespace Lattice {
namespace Viewport {

ItemPicker::ItemPicker() {
}

void ItemPicker::update(const SP<Document::Item> &rootItem) {
    addItemRecursive(rootItem);
}

SP<Document::Item> ItemPicker::pick(const Ray<float> &worldMouseRay) const {
    Q_UNUSED(worldMouseRay);
    // TODO
    return {};
}

void ItemPicker::addItemRecursive(const SP<Document::Item> &item) {
    if (auto meshItem = std::dynamic_pointer_cast<Document::MeshItem>(item); meshItem) {
        _items.push_back(meshItem);
    }
    for (auto& child : item->childItems()) {
        addItemRecursive(child);
    }
}

std::tuple<bool, float> ItemPicker::intersectsRayMesh(const Ray<float> &ray, const SP<Document::Mesh> &mesh) {
    Q_UNUSED(ray); Q_UNUSED(mesh);
    // TODO
    return {false, 0};
}

}
} // namespace Lattice
