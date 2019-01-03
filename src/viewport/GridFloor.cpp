#include "GridFloor.hpp"
#include "Operations.hpp"
#include "../gl/LineVAO.hpp"
#include "../gl/VertexBuffer.hpp"
#include "../support/Camera.hpp"

using namespace glm;

namespace Lattice::Viewport {

GridFloor::GridFloor() {
    auto buffer = std::make_shared<GL::VertexBuffer>();
    _vao = std::make_shared<GL::LineVAO>(buffer);

    // build grid
    constexpr int count = 200;
    constexpr double unit = 1;

    std::vector<GL::VertexBuffer::Vertex> vertices;
    std::vector<std::vector<uint32_t>> lineStrips;

    for (int z = -count; z <= count; ++z) {
        dvec3 v1(-count*unit, 0, z*unit);
        dvec3 v2(count*unit, 0, z*unit);
        auto i1 = uint32_t(vertices.size());
        vertices.push_back({v1, {}, {}});
        auto i2 = uint32_t(vertices.size());
        vertices.push_back({v2, {}, {}});
        lineStrips.push_back({i1, i2});
    }
    for (int x = -count; x <= count; ++x) {
        dvec3 v1(x*unit, 0, -count*unit);
        dvec3 v2(x*unit, 0, count*unit);
        auto i1 = uint32_t(vertices.size());
        vertices.push_back({v1, {}, {}});
        auto i2 = uint32_t(vertices.size());
        vertices.push_back({v2, {}, {}});
        lineStrips.push_back({i1, i2});
    }

    buffer->setVertices(vertices);
    _vao->setLineStrips(lineStrips);
}

void GridFloor::draw(const SP<Operations> &operations, const Camera &camera) {
    operations->drawLine.draw(_vao, dmat4(1), camera, 1, dvec3(0.5));
}

} // namespace Lattice
