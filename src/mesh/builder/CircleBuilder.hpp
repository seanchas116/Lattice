#pragma once
#include "../Mesh.hpp"

namespace Lattice {
namespace Mesh {

class CircleBuilder {
public:
    Mesh build() const;

    glm::vec3 center{0};
    float radius{1};
    int segmentCount{16};
    int normalAxis{0};
    MaterialHandle material;
};

} // namespace Mesh
} // namespace Lattice

