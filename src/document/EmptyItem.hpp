#pragma once
#include "Item.hpp"

namespace Lattice {

class EmptyItem final : public Item {
    Q_OBJECT
public:
    EmptyItem();
    SP<Item> clone() const override;
};

} // namespace Shapecraft
