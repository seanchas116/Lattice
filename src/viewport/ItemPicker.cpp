#include "ItemPicker.hpp"
#include "../document/Item.hpp"
#include "../document/Mesh.hpp"
#include "../document/MeshItem.hpp"
#include "../support/ScopedTimer.hpp"
#include <map>

namespace Lattice {
namespace Viewport {

ItemPicker::ItemPicker() {
}

void ItemPicker::update(const SP<Document::Item> &rootItem) {
    _entries.clear();
    addItemRecursive(rootItem);
}

std::tuple<SP<Document::Item>, float> ItemPicker::pick(const Ray<float> &worldMouseRay) const {
    std::map<float, SP<Document::Item>> intersectingItems;
    for (auto& entry : _entries) {
        glm::mat4 inverseModelMatrix = glm::inverse(entry.item->location().matrix());
        Ray<float> modelMouseRay((inverseModelMatrix * glm::vec4(worldMouseRay.origin, 1)).xyz,
                                 (inverseModelMatrix * glm::vec4(worldMouseRay.direction, 0)).xyz);

        if (!entry.box.intersects(modelMouseRay)) {
            continue;
        }
        auto [intersects, t] = intersectsRayMesh(modelMouseRay, entry.item->mesh());
        if (intersects) {
            intersectingItems[t] = entry.item;
        }
    }

    if (intersectingItems.empty()) {
        return {};
    }
    auto nearest = intersectingItems.begin();
    return {nearest->second, nearest->first};
}

void ItemPicker::addItemRecursive(const SP<Document::Item> &item) {
    if (auto meshItem = std::dynamic_pointer_cast<Document::MeshItem>(item); meshItem) {
        _entries.push_back({meshItem, meshItem->mesh()->boundingBox()});
    }
    for (auto& child : item->childItems()) {
        addItemRecursive(child);
    }
}

std::tuple<bool, float> ItemPicker::intersectsRayMesh(const Ray<float> &ray, const SP<Document::Mesh> &mesh) {
    ScopedTimer timer("intersectsRayMesh");
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
