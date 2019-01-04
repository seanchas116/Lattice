#include "ItemPicker.hpp"
#include "../document/Item.hpp"
#include "../document/Mesh.hpp"
#include "../document/MeshItem.hpp"
#include <map>

namespace Lattice {
namespace Viewport {

ItemPicker::ItemPicker() {
}

void ItemPicker::update(const SP<Document::Item> &rootItem) {
    addItemRecursive(rootItem);
}

SP<Document::Item> ItemPicker::pick(const Ray<float> &worldMouseRay) const {
    Q_UNUSED(worldMouseRay);

    std::map<float, SP<Document::Item>> intersectingItems;
    for (auto& item : _items) {
        // TODO: use bounding box
        auto [intersects, t] = intersectsRayMesh(worldMouseRay, item->mesh());
        if (intersects) {
            intersectingItems[t] = item;
        }
    }

    if (intersectingItems.empty()) {
        return {};
    }
    return intersectingItems.rbegin()->second; // nearset item
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
    // TODO: Use Bounding Volume Hierarchy to do faster
    for (auto& [_, f] : mesh->faces()) {
        auto v0 = f->vertices()[0]->position();
        for (uint32_t i = 2; i < uint32_t(f->vertices().size()); ++i) {
            auto v1 = f->vertices()[i - 1]->position();
            auto v2 = f->vertices()[i]->position();

            auto [intersects, t] = ray.intersectsTriangle({v0, v1, v2});
            if (intersects) {
                return {true, t};
            }
        }
    }
    return {false, 0};
}

}
} // namespace Lattice
