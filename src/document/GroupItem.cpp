#include "GroupItem.hpp"

namespace Lattice {

GroupItem::GroupItem() {
}

SP<Item> GroupItem::clone() const {
    auto cloned = std::make_shared<GroupItem>();
    return cloned;
}

bool GroupItem::canInsertItem(const SP<const Item> &item) const {
    Q_UNUSED(item)
    return true;
}

} // namespace Shapecraft
