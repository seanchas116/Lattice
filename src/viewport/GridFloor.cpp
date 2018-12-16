#include "GridFloor.hpp"
#include "../gl/LineMesh.hpp"
#include "../gl/VertexBuffer.hpp"

namespace Lattice {

GridFloor::GridFloor() {
    auto buffer = std::make_shared<VertexBuffer>();
    _mesh = std::make_shared<LineMesh>(buffer);

    // build grid
    constexpr int count = 10;
    constexpr float unit = 1;

    std::vector<VertexBuffer::Vertex> vertices;
    std::vector<std::vector<uint32_t>> lineStrips;

    for (int z = -count; z <= count; ++z) {
        glm::vec3 v1(-count*unit, 0, z*unit);
        glm::vec3 v2(count*unit, 0, z*unit);
        auto i1 = uint32_t(vertices.size());
        vertices.push_back({v1, {}, {}});
        auto i2 = uint32_t(vertices.size());
        vertices.push_back({v2, {}, {}});
        lineStrips.push_back({i1, i2});
    }
    for (int x = -count; x <= count; ++x) {
        glm::vec3 v1(x*unit, 0, -count*unit);
        glm::vec3 v2(x*unit, 0, count*unit);
        auto i1 = uint32_t(vertices.size());
        vertices.push_back({v1, {}, {}});
        auto i2 = uint32_t(vertices.size());
        vertices.push_back({v2, {}, {}});
        lineStrips.push_back({i1, i2});
    }

    buffer->setVertices(vertices);
    _mesh->setLineStrips(lineStrips);
}

void GridFloor::draw() {
    _mesh->draw();
}

} // namespace Lattice
