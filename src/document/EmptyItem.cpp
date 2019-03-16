#include "EmptyItem.hpp"

namespace Lattice {
namespace Document {

EmptyItem::EmptyItem() {
}

SP<Item> EmptyItem::clone() const {
    auto cloned = makeShared<EmptyItem>();
    return cloned;
}

}
} // namespace Lattice
