#pragma once
#include <unordered_set>

namespace Lattice {

template <typename T>
bool includes(const std::unordered_set<T>& set, const std::unordered_set<T>& otherSet) {
    for (auto& value : otherSet) {
        if (set.find(value) == set.end()) {
            return false;
        }
    }
    return true;
}

} // namespace Lattice
