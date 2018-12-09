#pragma once

#include <glm/glm.hpp>
#include <QDebug>
#include <QtDebug>
#include <array>

namespace Lattice {

QDebug operator<<(QDebug debug, const glm::vec2& v);
QDebug operator<<(QDebug debug, const glm::vec3& v);
QDebug operator<<(QDebug debug, const glm::vec4& v);

template <typename T, size_t N>
QDebug operator<<(QDebug debug, const std::array<T, N>& array) {
    debug << "[";
    for (size_t i = 0; i < array.size(); ++i) {
        if (i != 0) {
            debug << ",";
        }
        debug << array[i];
    }
    debug << "]";
    return debug;
}

} // namespace Shapecraft
