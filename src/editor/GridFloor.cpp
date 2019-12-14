#include "GridFloor.hpp"
#include "../draw/Vertex.hpp"
#include "../gl/VAO.hpp"
#include "../gl/VertexBuffer.hpp"
#include "../support/Camera.hpp"
#include "CameraState.hpp"
#include "EditorViewport.hpp"

using namespace glm;

namespace Lattice {
namespace Editor {

GridFloor::GridFloor() : _vbo(makeShared<GL::VertexBuffer<Draw::PointLineVertex>>()),
                         _indexBuffer(makeShared<GL::IndexBuffer>()),
                         _vao(makeShared<GL::VAO>(_vbo, _indexBuffer)),
                         _yAxisIndexBuffer(makeShared<GL::IndexBuffer>()),
                         _zAxisIndexBuffer(makeShared<GL::IndexBuffer>()),
                         _yAxisVAO(makeShared<GL::VAO>(_vbo, _yAxisIndexBuffer)),
                         _zAxisVAO(makeShared<GL::VAO>(_vbo, _zAxisIndexBuffer)) {
    // build grid
    constexpr int count = 200;
    constexpr double unit = 1;

    std::vector<Draw::PointLineVertex> vertices;
    std::vector<std::vector<uint32_t>> lineStrips;
    std::vector<uint32_t> yLineStrip;
    std::vector<uint32_t> zLineStrip;

    for (int z = -count; z <= count; ++z) {
        dvec3 v1(0, -count * unit, z * unit);
        dvec3 v2(0, count * unit, z * unit);
        auto i1 = uint32_t(vertices.size());
        vertices.push_back({v1, vec4(0), 1});
        auto i2 = uint32_t(vertices.size());
        vertices.push_back({v2, vec4(0), 1});

        if (z == 0) {
            yLineStrip = {i1, i2};
        } else {
            lineStrips.push_back({i1, i2});
        }
    }
    for (int y = -count; y <= count; ++y) {
        dvec3 v1(0, y * unit, -count * unit);
        dvec3 v2(0, y * unit, count * unit);
        auto i1 = uint32_t(vertices.size());
        vertices.push_back({v1, vec4(0), 1});
        auto i2 = uint32_t(vertices.size());
        vertices.push_back({v2, vec4(0), 1});

        if (y == 0) {
            zLineStrip = {i1, i2};
        } else {
            lineStrips.push_back({i1, i2});
        }
    }

    _vbo->setVertices(vertices);
    _indexBuffer->setLineStrips(lineStrips);
    _yAxisIndexBuffer->setLineStrips({yLineStrip});
    _zAxisIndexBuffer->setLineStrips({zLineStrip});
}

void GridFloor::draw(const Viewport::DrawEvent &event) {
    int normalAxis = 1;

    auto viewport = static_cast<EditorViewport *>(event.viewport);
    auto cameraState = viewport->cameraState();

    if (cameraState->projection() == Camera::Projection::Orthographic) {
        auto orientation = cameraState->orientation();

        switch (orientation) {
        case CameraState::Orientation::Front:
        case CameraState::Orientation::Back:
            normalAxis = 2;
            break;
        case CameraState::Orientation::Left:
        case CameraState::Orientation::Right:
            normalAxis = 0;
            break;
        default:
            break;
        }
    }

    static const std::array<glm::mat4, 3> swizzleTransforms{
        glm::mat4(1),                                              // xyz to xyz
        glm::mat4(0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1), // xyz to yzx
        glm::mat4(0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1), // xyz to zxy
    };
    auto transform = swizzleTransforms[normalAxis];

    event.operations->drawLine.draw(_vao, transform, event.camera, 1, vec4(0.5, 0.5, 0.5, 1));

    int axis0 = (1 + normalAxis) % 3;
    int axis1 = (2 + normalAxis) % 3;
    vec4 color0(0.5f);
    color0[axis0] = 0.8f;
    vec4 color1(0.5f);
    color1[axis1] = 0.8f;

    event.operations->drawLine.draw(_yAxisVAO, transform, event.camera, 2, color0);
    event.operations->drawLine.draw(_zAxisVAO, transform, event.camera, 2, color1);
}

} // namespace Editor
} // namespace Lattice
