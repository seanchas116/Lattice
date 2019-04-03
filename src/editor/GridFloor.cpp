#include "GridFloor.hpp"
#include "../gl/VAO.hpp"
#include "../gl/VertexBuffer.hpp"
#include "../gl/Vertex.hpp"
#include "../support/Camera.hpp"
#include "../drawable/LinesDrawable.hpp"

using namespace glm;

namespace Lattice {
namespace Editor {

GridFloor::GridFloor() : _drawable(makeShared<Drawable::LinesDrawable>()),
                         _xAxisDrawable(makeShared<Drawable::LinesDrawable>()),
                         _zAxisDrawable(makeShared<Drawable::LinesDrawable>())
{
    // build grid
    constexpr int count = 200;
    constexpr double unit = 1;

    std::vector<Drawable::Point> vertices;
    std::vector<std::vector<uint32_t>> lineStrips;
    std::vector<uint32_t> xLineStrip;
    std::vector<uint32_t> zLineStrip;

    for (int z = -count; z <= count; ++z) {
        dvec3 v1(-count*unit, 0, z*unit);
        dvec3 v2(count*unit, 0, z*unit);
        auto i1 = uint32_t(vertices.size());
        vertices.push_back({v1, {}});
        auto i2 = uint32_t(vertices.size());
        vertices.push_back({v2, {}});

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
        vertices.push_back({v1, {}});
        auto i2 = uint32_t(vertices.size());
        vertices.push_back({v2, {}});

        if (x == 0) {
            zLineStrip = {i1, i2};
        } else {
            lineStrips.push_back({i1, i2});
        }
    }

    _drawable->setPoints(vertices);
    _xAxisDrawable->setPoints(vertices);
    _zAxisDrawable->setPoints(vertices);

    _drawable->setLineStrips(lineStrips);
    _xAxisDrawable->setLineStrips({xLineStrip});
    _zAxisDrawable->setLineStrips({zLineStrip});

    _drawable->setUseVertexColor(false);
    _drawable->setColor(vec4(0.5, 0.5, 0.5, 1));
    _xAxisDrawable->setUseVertexColor(false);
    _xAxisDrawable->setColor(vec4(0.8, 0.5, 0.5, 1));
    _zAxisDrawable->setUseVertexColor(false);
    _zAxisDrawable->setColor(vec4(0.5, 0.5, 0.8, 1));
}

void GridFloor::draw(const SP<Render::Operations> &operations, const SP<Camera> &camera) {
    Q_UNUSED(operations);
    _drawable->draw(operations->resourceBag, dmat4(1), camera);
    _xAxisDrawable->draw(operations->resourceBag, dmat4(1), camera);
    _zAxisDrawable->draw(operations->resourceBag, dmat4(1), camera);
}

}
} // namespace Lattice
