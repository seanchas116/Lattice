#pragma once
#include <glm/glm.hpp>

namespace Lattice {
namespace GL {

struct Vertex {
    glm::vec3 position {0};
    glm::vec2 texCoord {0};
    glm::vec3 normal {0};
    glm::vec4 color {0};
};

}
}