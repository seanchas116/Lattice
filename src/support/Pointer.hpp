#pragma once

#include <memory>
#include "SharedPointer.hpp"

namespace Lattice {

template <class T>
using SP = std::shared_ptr<T>;

template <class T>
using WP = std::weak_ptr<T>;

} // namespace Lattice
