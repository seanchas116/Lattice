#include "Manipulator.hpp"
#include "Operations.hpp"
#include "../gl/VAO.hpp"
#include "../gl/VertexBuffer.hpp"

using namespace glm;

namespace Lattice {

Manipulator::Manipulator() {
    _vao = std::make_shared<VAO>();

    constexpr float headLength = 0.5f;
    constexpr float headWidth = 0.5f;
    constexpr float bodyLength = 0.5f;
    constexpr uint32_t headSegmentCount = 16;

    std::vector<VertexBuffer::Vertex> vertices;
    std::vector<VAO::Triangle> triangles;

    float headRadius = headWidth * 0.5f;

    for (uint32_t i = 0; i < headSegmentCount; ++i) {
        float angle = 2.f * float(M_PI) / float(headSegmentCount) * float(i);
        vec3 pos(cos(angle) * headRadius, sin(angle) * headRadius, bodyLength);
        vertices.push_back({pos, {}, {}});
    }
    vertices.push_back({vec3(0, 0, bodyLength), {}, {}});
    vertices.push_back({vec3(0, 0, bodyLength + headLength), {}, {}});

    for (uint32_t i = 0; i < headSegmentCount; ++i) {
        auto index0 = i;
        auto index1 = (i + 1) % headSegmentCount;
        auto indexBottom = headSegmentCount;
        auto indexTop = headSegmentCount + 1;

        triangles.push_back({index0, index1, indexTop});
        triangles.push_back({index1, index0, indexBottom});
    }

    _vao->vertexBuffer()->setVertices(vertices);
    _vao->setTriangles(triangles);
}

void Manipulator::draw(const SP<Operations> &operations, const glm::mat4 &viewMatrix, const Projection &projection) {
    operations->drawSolid.draw(_vao, viewMatrix, projection, vec3(0), vec3(1));
}

} // namespace Lattice
