#include "EmptyItem.hpp"

namespace Lattice {
namespace Document {

EmptyItem::EmptyItem() {
}

SP<Object> EmptyItem::clone() const {
    auto cloned = makeShared<EmptyItem>();
    return cloned;
}

}
} // namespace Lattice
