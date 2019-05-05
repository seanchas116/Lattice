#pragma once
#include <glm/glm.hpp>
#include <QImage>

namespace Lattice {
namespace Draw {

struct Material {
    glm::vec3 baseColor {1, 0, 0};
    QImage baseColorImage;

    float metallic {0};
    QImage metallicImage;

    float roughness {0.5};
    QImage roughnessImage;
};

}
}
