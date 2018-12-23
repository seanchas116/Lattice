#include "Manipulator.hpp"
#include "Operations.hpp"
#include "../gl/VAO.hpp"
#include "../gl/LineVAO.hpp"
#include "../gl/VertexBuffer.hpp"

using namespace glm;

namespace Lattice {

namespace {

constexpr float bodyLength = 2.f;
constexpr float bodyWidth = 1.f;
constexpr float headLength = 0.4f;
constexpr float headWidth = 0.2f;
constexpr uint32_t headSegmentCount = 16;

}

Manipulator::Manipulator() {
    {
        _headVAO = std::make_shared<VAO>();

        std::vector<VertexBuffer::Vertex> vertices;
        std::vector<VAO::Triangle> triangles;

        float headRadius = headWidth * 0.5f;

        for (uint32_t i = 0; i < headSegmentCount; ++i) {
            float angle = 2.f * float(M_PI) / float(headSegmentCount) * float(i);
            vec3 pos(bodyLength, cos(angle) * headRadius, sin(angle) * headRadius);
            vertices.push_back({pos, {}, {}});
        }
        vertices.push_back({vec3(bodyLength, 0, 0), {}, {}});
        vertices.push_back({vec3(bodyLength + headLength, 0, 0), {}, {}});

        for (uint32_t i = 0; i < headSegmentCount; ++i) {
            auto index0 = i;
            auto index1 = (i + 1) % headSegmentCount;
            auto indexBottom = headSegmentCount;
            auto indexTop = headSegmentCount + 1;

            triangles.push_back({index0, index1, indexTop});
            triangles.push_back({index1, index0, indexBottom});
        }

        _headVAO->vertexBuffer()->setVertices(vertices);
        _headVAO->setTriangles(triangles);
    }

    {
        _bodyVAO = std::make_shared<LineVAO>();

        std::vector<VertexBuffer::Vertex> bodyVertices;
        std::vector<std::vector<uint32_t>> bodyLineStrips;

        bodyVertices.push_back({vec3(0.1, 0, 0), {}, {}});
        bodyVertices.push_back({vec3(bodyLength, 0, 0), {}, {}});
        bodyLineStrips = { {0, 1} };

        _bodyVAO->vertexBuffer()->setVertices(bodyVertices);
        _bodyVAO->setLineStrips(bodyLineStrips);
    }
}

void Manipulator::draw(const SP<Operations> &operations, const glm::mat4 &viewMatrix, const Projection &projection) {
    operations->drawSolid.draw(_headVAO, viewMatrix, projection, vec3(0), vec3(1));
    operations->drawLine.draw(_bodyVAO, viewMatrix, projection, bodyWidth, vec3(1));
}

} // namespace Lattice
