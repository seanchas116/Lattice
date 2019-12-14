#pragma once
#include <QImage>
#include <glm/glm.hpp>

namespace Lattice {
namespace Draw {

struct Material {
    glm::vec3 baseColor{1, 0, 0};
    QImage baseColorImage;

    float metallic{0};
    QImage metallicImage;

    float roughness{0.5};
    QImage roughnessImage;
};

} // namespace Draw
} // namespace Lattice
