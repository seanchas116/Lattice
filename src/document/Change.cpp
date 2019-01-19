#include "Change.hpp"

namespace Lattice {
namespace Document {

Change::~Change() = default;

bool Change::mergeWith(const SP<const Change> &other) {
    Q_UNUSED(other)
    return false;
}

} // namespace Document
} // namespace Lattice
