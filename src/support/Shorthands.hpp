#pragma once

#include <memory>
#include "SharedPointer.hpp"
#include "OptionalGuard.hpp"

namespace Lattice {

template <class T>
using SP = SharedPointer<T>;

template <class T>
using WP = WeakPointer<T>;

} // namespace Lattice
