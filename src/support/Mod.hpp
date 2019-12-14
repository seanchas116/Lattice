#pragma once
#include <type_traits>

namespace Lattice {

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
T mod(T x, T n) {
    return (x % n + n) % n;
}

} // namespace Lattice
