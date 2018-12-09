#pragma once
#include "Item.hpp"

namespace Lattice {

class GroupItem final : public Item {
    Q_OBJECT
public:
    GroupItem();
    SP<Item> clone() const override;
    bool canInsertItem(const SP<const Item>& item) const override;
};

} // namespace Shapecraft
