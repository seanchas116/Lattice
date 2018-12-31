#include "Manipulator.hpp"
#include "Operations.hpp"
#include "../gl/VAO.hpp"
#include "../gl/LineVAO.hpp"
#include "../gl/VertexBuffer.hpp"
#include "../support/Debug.hpp"
#include "../support/Line.hpp"
#include <array>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

namespace Lattice {

namespace {

constexpr float bodyLength = 2.f;
constexpr float bodyWidth = 2.f;
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

void Manipulator::draw(const SP<Operations> &operations, const Camera &camera) {
    auto [manipulatorToWorld, isInScreen] = this->manipulatorToWorldMatrix(vec3(0), camera);
    if (!isInScreen){
        return;
    }

    auto transforms = std::array<mat4, 3> {
            mat4(1), // x
            mat4(0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1), // y
            mat4(0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1), // z
    };
    auto colors = std::array<vec3, 3> {
            vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1)
    };

    for (size_t i = 0; i < 3; ++i) {
        operations->drawSolid.draw(_headVAO, manipulatorToWorld * transforms[i], camera, vec3(0), colors[i]);
        operations->drawLine.draw(_bodyVAO, manipulatorToWorld * transforms[i], camera, bodyWidth, colors[i]);
    }
}

bool Manipulator::mousePress(QMouseEvent *event, vec2 pos, const Camera &camera) {
    auto [manipulatorToWorld, isInScreen] = this->manipulatorToWorldMatrix(vec3(0), camera);
    if (!isInScreen) {
        return false;
    }

    qDebug() << "press at" << pos;
    vec3 front = camera.screenToWorld(vec3(pos, 0));
    vec3 back = camera.screenToWorld(vec3(pos, 1));

    qDebug() << front << back;

    mat4 manipulatorToCamera = camera.worldToCameraMatrix() * manipulatorToWorld;

    vec3 arrowXDirection = manipulatorToCamera[0].xyz;
    vec3 arrowCenter = manipulatorToCamera[3].xyz;

    Line mouseRay(front, back);
    Line arrowRay(arrowCenter, arrowCenter + arrowXDirection);

    qDebug() << "distance" << LineLineDistance(mouseRay, arrowRay).distance;

    return false;
}

bool Manipulator::mouseMove(QMouseEvent *event, vec2 pos, const Camera &camera) {
    return false;
}

bool Manipulator::mouseRelease(QMouseEvent *event, vec2 pos, const Camera &camera) {
    return false;
}

std::pair<mat4, bool> Manipulator::manipulatorToWorldMatrix(vec3 targetPosition, const Camera &camera) const {
    auto [screenPos, isInScreen] = camera.worldToScreen(targetPosition);
    if (!isInScreen){
        return {mat4(), false};
    }
    vec3 screenPosFixedDepth(screenPos.xy, 0.5f);
    vec3 positionFixedDepth_worldSpace = camera.screenToWorld(screenPosFixedDepth);

    float scale = 1.f / float(camera.viewSize().y) * 10.f;

    mat4 modelMatrix = glm::scale(glm::translate(glm::mat4(1), positionFixedDepth_worldSpace), vec3(scale));
    return {modelMatrix, true};
}

} // namespace Lattice
