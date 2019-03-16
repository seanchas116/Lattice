#include "GridFloor.hpp"
#include "../gl/VAO.hpp"
#include "../gl/VertexBuffer.hpp"
#include "../support/Camera.hpp"

using namespace glm;

namespace Lattice {
namespace Editor {

GridFloor::GridFloor() :
    _vbo(makeShared<GL::VertexBuffer<GL::Vertex>>()),
    _indexBuffer(makeShared<GL::IndexBuffer>()),
    _vao(makeShared<GL::VAO>(_vbo, _indexBuffer)),
    _xAxisIndexBuffer(makeShared<GL::IndexBuffer>()),
    _zAxisIndexBuffer(makeShared<GL::IndexBuffer>()),
    _xAxisVAO(makeShared<GL::VAO>(_vbo, _xAxisIndexBuffer)),
    _zAxisVAO(makeShared<GL::VAO>(_vbo, _zAxisIndexBuffer))
{
    // build grid
    constexpr int count = 200;
    constexpr double unit = 1;

    std::vector<GL::Vertex> vertices;
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
    _indexBuffer->setLineStrips(lineStrips);
    _xAxisIndexBuffer->setLineStrips({xLineStrip});
    _zAxisIndexBuffer->setLineStrips({zLineStrip});
}

void GridFloor::draw(const SP<Render::Operations> &operations, const SP<Camera> &camera) {
    operations->drawLine.draw(_vao, dmat4(1), camera, 1, vec4(0.5, 0.5, 0.5, 1));
    operations->drawLine.draw(_xAxisVAO, dmat4(1), camera, 1, vec4(0.8, 0.5, 0.5, 1));
    operations->drawLine.draw(_zAxisVAO, dmat4(1), camera, 1, vec4(0.5, 0.5, 0.8, 1));
}

}
} // namespace Lattice
