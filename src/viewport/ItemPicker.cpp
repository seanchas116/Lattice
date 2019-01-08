#include "ItemPicker.hpp"
#include "MeshPicker.hpp"
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

std::optional<std::pair<SP<Document::Item>, float> > ItemPicker::pick(const Ray<float> &worldMouseRay) const {
    std::map<float, SP<Document::Item>> intersectingItems;
    for (auto& entry : _entries) {
        glm::mat4 inverseModelMatrix = glm::inverse(entry.item->location().matrix());
        Ray<float> modelMouseRay((inverseModelMatrix * glm::vec4(worldMouseRay.origin, 1)).xyz,
                                 (inverseModelMatrix * glm::vec4(worldMouseRay.direction, 0)).xyz);

        if (!entry.box.intersects(modelMouseRay)) {
            continue;
        }
        MeshPicker picker(entry.item->mesh());
        LATTICE_OPTIONAL_LET(result, picker.picKFace(modelMouseRay),
            auto [face, t] = result;
            intersectingItems.insert({t, entry.item});
        )
    }

    if (intersectingItems.empty()) {
        return {};
    }
    auto nearest = intersectingItems.begin();
    return {{nearest->second, nearest->first}};
}

void ItemPicker::addItemRecursive(const SP<Document::Item> &item) {
    LATTICE_OPTIONAL_LET(meshItem, dynamicPointerCast<Document::MeshItem>(item),
        _entries.push_back({meshItem, meshItem->mesh()->boundingBox()});
    )
    for (auto& child : item->childItems()) {
        addItemRecursive(child);
    }
}

}
} // namespace Lattice
