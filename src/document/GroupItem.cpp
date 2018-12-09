#include "GroupItem.hpp"

namespace Lattice {

GroupItem::GroupItem() {
}

SP<Item> GroupItem::clone() const {
    auto cloned = std::make_shared<GroupItem>();
    return cloned;
}

} // namespace Shapecraft
