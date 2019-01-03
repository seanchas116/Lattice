#pragma once
#include "Item.hpp"

namespace Lattice::Document {

class EmptyItem final : public Item {
    Q_OBJECT
public:
    EmptyItem();
    SP<Item> clone() const override;
};

} // namespace Lattice
