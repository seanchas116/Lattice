#include "GridFloor.hpp"
#include "Shaders.hpp"
#include "../gl/LineVAO.hpp"
#include "../gl/VertexBuffer.hpp"
#include "../support/Projection.hpp"
#include "../support/Camera.hpp"

using namespace glm;

namespace Lattice {

GridFloor::GridFloor() {
    auto buffer = std::make_shared<VertexBuffer>();
    _vao = std::make_shared<LineVAO>(buffer);

    // build grid
    constexpr int count = 10;
    constexpr float unit = 1;

    std::vector<VertexBuffer::Vertex> vertices;
    std::vector<std::vector<uint32_t>> lineStrips;

    for (int z = -count; z <= count; ++z) {
        vec3 v1(-count*unit, 0, z*unit);
        vec3 v2(count*unit, 0, z*unit);
        auto i1 = uint32_t(vertices.size());
        vertices.push_back({v1, {}, {}});
        auto i2 = uint32_t(vertices.size());
        vertices.push_back({v2, {}, {}});
        lineStrips.push_back({i1, i2});
    }
    for (int x = -count; x <= count; ++x) {
        vec3 v1(x*unit, 0, -count*unit);
        vec3 v2(x*unit, 0, count*unit);
        auto i1 = uint32_t(vertices.size());
        vertices.push_back({v1, {}, {}});
        auto i2 = uint32_t(vertices.size());
        vertices.push_back({v2, {}, {}});
        lineStrips.push_back({i1, i2});
    }

    buffer->setVertices(vertices);
    _vao->setLineStrips(lineStrips);
}

void GridFloor::draw(const SP<Shaders> &shaders, const Camera &camera, const Projection &projection) {
    shaders->drawLine.draw(_vao, camera.matrix(), projection, 1, vec3(0.5));
}

} // namespace Lattice
