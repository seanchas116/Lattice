#include "GridFloor.hpp"
#include "Operations.hpp"
#include "../gl/LineVAO.hpp"
#include "../gl/VertexBuffer.hpp"
#include "../support/Camera.hpp"

using namespace glm;

namespace Lattice::Viewport {

GridFloor::GridFloor() :
    _vbo(makeShared<GL::VertexBuffer>()),
    _vao(makeShared<GL::LineVAO>(_vbo)),
    _xAxisVAO(makeShared<GL::LineVAO>(_vbo)),
    _zAxisVAO(makeShared<GL::LineVAO>(_vbo))
{
    // build grid
    constexpr int count = 200;
    constexpr double unit = 1;

    std::vector<GL::VertexBuffer::Vertex> vertices;
    std::vector<std::vector<uint32_t>> lineStrips;
    std::vector<uint32_t> xLineStrip;
    std::vector<uint32_t> zLineStrip;

    for (int z = -count; z <= count; ++z) {
        dvec3 v1(-count*unit, 0, z*unit);
        dvec3 v2(count*unit, 0, z*unit);
        auto i1 = uint32_t(vertices.size());
        vertices.push_back({v1, {}, {}});
        auto i2 = uint32_t(vertices.size());
        vertices.push_back({v2, {}, {}});

        if (z == 0) {
            xLineStrip = {i1, i2};
        } else {
            lineStrips.push_back({i1, i2});
        }
    }
    for (int x = -count; x <= count; ++x) {
        dvec3 v1(x*unit, 0, -count*unit);
        dvec3 v2(x*unit, 0, count*unit);
        auto i1 = uint32_t(vertices.size());
        vertices.push_back({v1, {}, {}});
        auto i2 = uint32_t(vertices.size());
        vertices.push_back({v2, {}, {}});

        if (x == 0) {
            zLineStrip = {i1, i2};
        } else {
            lineStrips.push_back({i1, i2});
        }
    }

    _vbo->setVertices(vertices);
    _vao->setLineStrips(lineStrips);
    _xAxisVAO->setLineStrips({xLineStrip});
    _zAxisVAO->setLineStrips({zLineStrip});
}

void GridFloor::draw(const SP<Operations> &operations, const Camera &camera) {
    operations->drawLine.draw(_vao, dmat4(1), camera, 1, dvec3(0.5));
    operations->drawLine.draw(_xAxisVAO, dmat4(1), camera, 1, dvec3(0.8, 0.5, 0.5));
    operations->drawLine.draw(_zAxisVAO, dmat4(1), camera, 1, dvec3(0.5, 0.5, 0.8));
}

} // namespace Lattice
