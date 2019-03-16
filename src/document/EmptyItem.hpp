#pragma once
#include "Item.hpp"

namespace Lattice {
namespace Document {

class EmptyItem final : public Item {
    Q_OBJECT
public:
    EmptyItem();
    SP<Item> clone() const override;
};

}
} // namespace Lattice
