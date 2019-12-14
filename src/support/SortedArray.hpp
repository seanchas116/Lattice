#pragma once
#include "Hash.hpp"
#include <array>

namespace Lattice {

template <typename T, std::size_t N>
class SortedArray {
  public:
    using internal_container = std::array<T, N>;
    using value_type = typename internal_container::value_type;
    using size_type = typename internal_container::size_type;
    using difference_type = typename internal_container::difference_type;
    using reference = typename internal_container::reference;
    using const_reference = typename internal_container::const_reference;
    using pointer = typename internal_container::pointer;
    using const_pointer = typename internal_container::const_pointer;
    using iterator = typename internal_container::iterator;
    using const_iterator = typename internal_container::const_iterator;
    using reverse_iterator = typename internal_container::reverse_iterator;
    using const_reverse_iterator = typename internal_container::const_reverse_iterator;

    template <class... Args, std::enable_if_t<(2 <= sizeof...(Args))> * = nullptr>
    SortedArray(Args &&... args) : _array{std::forward<Args>(args)...} {
        std::sort(_array.begin(), _array.end());
    }
    SortedArray(const internal_container &array) : _array(array) {
        std::sort(_array.begin(), _array.end());
    }
    SortedArray(internal_container &&array) : _array(array) {
        std::sort(_array.begin(), _array.end());
    }

    const auto &array() const { return _array; }

    const auto &at(size_type i) const { return _array.at(i); }
    const auto &operator[](size_type i) const { return _array[i]; }

    constexpr auto size() const { return _array.size(); }

    auto begin() const { return _array.begin(); }
    auto end() const { return _array.end(); }
    auto rbegin() const { return _array.rbegin(); }
    auto rend() const { return _array.rend(); }

    template <typename U>
    bool operator==(const SortedArray<U, N> &other) const {
        return _array == other.array();
    }

  private:
    internal_container _array;
};

} // namespace Lattice

namespace std {
template <typename T, std::size_t N>
struct hash<Lattice::SortedArray<T, N>> {
    size_t operator()(const Lattice::SortedArray<T, N> &xs) const {
        return hash<std::array<T, N>>()(xs.array());
    }
};
} // namespace std
