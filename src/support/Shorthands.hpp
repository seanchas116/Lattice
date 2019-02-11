#pragma once

#include "SharedPointer.hpp"
#include "OptionalGuard.hpp"

namespace Lattice {

template <class T>
using SP = SharedPointer<T>;

template <class T>
using WP = WeakPointer<T>;

template <class T>
using Opt = std::optional<T>;

template <class T>
using Fn = std::function<T>;

} // namespace Lattice
