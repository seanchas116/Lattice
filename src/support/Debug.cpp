#include "Debug.hpp"

namespace Lattice {

QDebug operator<<(QDebug debug, const glm::vec2 &v) {
    QDebugStateSaver saver(debug);
    debug.nospace() << '(' << v.x << ", " << v.y << ')';
    return debug;
}

QDebug operator<<(QDebug debug, const glm::vec3 &v) {
    QDebugStateSaver saver(debug);
    debug.nospace() << '(' << v.x << ", " << v.y << ", " << v.z << ')';
    return debug;
}

QDebug operator<<(QDebug debug, const glm::vec4 &v) {
    QDebugStateSaver saver(debug);
    debug.nospace() << '(' << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ')';
    return debug;
}

} // namespace Shapecraft
