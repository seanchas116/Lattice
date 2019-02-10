#pragma once

#include <array>
#include <glm/glm.hpp>

namespace Lattice {
namespace Editor {
namespace Manipulator {

namespace Constants {
    constexpr double bodyBegin = 0.2;
    constexpr double bodyWidth = 2.0;
    constexpr double scaleHandleSize = 0.2;
    constexpr double translateHandleLength = 0.4;
    constexpr double translateHandleWidth = 0.2;
    constexpr double hitRadius = 0.2;
    constexpr double fixedDepth = 0.75;

    inline const std::array<glm::dvec3, 3> colors {
        glm::dvec3(1, 0, 0),
        glm::dvec3(0, 1, 0),
        glm::dvec3(0, 0, 1),
    };

    inline const std::array<glm::dvec3, 3> hoverColors {
        glm::dvec3(1, 0.5, 0.5),
        glm::dvec3(0.5, 1, 0.5),
        glm::dvec3(0.5, 0.5, 1),
    };

    inline const std::array<glm::dmat4, 3> swizzleTransforms {
        glm::dmat4(1), // x
        glm::dmat4(0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1), // y
        glm::dmat4(0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1), // z
    };
}

}
} // namespace Editor
} // namespace Lattice
