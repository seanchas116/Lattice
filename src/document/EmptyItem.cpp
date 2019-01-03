#include "EmptyItem.hpp"

namespace Lattice::Document {

EmptyItem::EmptyItem() {
}

SP<Item> EmptyItem::clone() const {
    auto cloned = std::make_shared<EmptyItem>();
    return cloned;
}

} // namespace Lattice
