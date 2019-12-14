#pragma once

#include "Shorthands.hpp"
#include <QDebug>
#include <QtDebug>
#include <array>
#include <glm/glm.hpp>
#include <optional>

namespace Lattice {

template <glm::length_t L, typename T, glm::qualifier Q>
QDebug operator<<(QDebug debug, const glm::vec<L, T, Q> &v) {
    QDebugStateSaver saver(debug);
    debug.nospace();
    debug << "(";
    for (size_t i = 0; i < L; ++i) {
        if (i != 0) {
            debug << ", ";
        }
        debug << v[i];
    }
    debug << ")";
    return debug;
}

template <typename T, size_t N>
QDebug operator<<(QDebug debug, const std::array<T, N> &array) {
    QDebugStateSaver saver(debug);
    debug.nospace();
    debug << "[";
    for (size_t i = 0; i < array.size(); ++i) {
        if (i != 0) {
            debug << ", ";
        }
        debug << array[i];
    }
    debug << "]";
    return debug;
}

template <typename T>
QDebug operator<<(QDebug debug, const Opt<T> &optional) {
    if (optional) {
        return debug << *optional;
    } else {
        return debug << "nullopt";
    }
}

template <typename T>
QDebug operator<<(QDebug debug, const SP<T> &ptr) {
    QDebugStateSaver saver(debug);
    debug.nospace();
    debug << "SP(";
    debug << ptr.get();
    debug << ")";
    return debug;
}

} // namespace Lattice
