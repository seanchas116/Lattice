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
constexpr float hitRadius = 0.2f;

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
    auto [manipulatorToWorld, isInScreen] = this->manipulatorToWorldMatrix(camera);
    if (!isInScreen){
        return;
    }

    auto transforms = std::array<dmat4, 3> {
            mat4(1), // x
            mat4(0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1), // y
            mat4(0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1), // z
    };
    auto colors = std::array<dvec3, 3> {
            dvec3(1, 0, 0), dvec3(0, 1, 0), dvec3(0, 0, 1)
    };

    for (size_t i = 0; i < 3; ++i) {
        operations->drawSolid.draw(_headVAO, manipulatorToWorld * transforms[i], camera, vec3(0), colors[i]);
        operations->drawLine.draw(_bodyVAO, manipulatorToWorld * transforms[i], camera, bodyWidth, colors[i]);
    }
}

bool Manipulator::mousePress(QMouseEvent *event, vec2 pos, const Camera &camera) {
    auto [distance, t, isInScreen] = distanceFromArrow(pos, camera);
    if (!isInScreen) {
        return false;
    }

    if (0 <= t && t <= bodyLength + headLength && distance <= hitRadius) {
        _isDragging = true;
        _initialDragValue = t;
        emit onDragStart();

        return true;
    }

    return false;
}

bool Manipulator::mouseMove(QMouseEvent *event, vec2 pos, const Camera &camera) {
    if (!_isDragging) {
        return false;
    }

    auto [distance, t, isInScreen] = distanceFromArrow(pos, camera);
    if (!isInScreen) {
        return false;
    }
    emit onDrag(dvec3(t - _initialDragValue, 0, 0));
    return true;
}

bool Manipulator::mouseRelease(QMouseEvent *event, vec2 pos, const Camera &camera) {
    if (!_isDragging) {
        return false;
    }
    _isDragging = false;
    emit onDragEnd();
    return true;
}

std::pair<dmat4, bool> Manipulator::manipulatorToWorldMatrix(const Camera &camera) const {
    auto [screenPos, isInScreen] = camera.mapWorldToScreen(_targetPosition);
    if (!isInScreen){
        return {mat4(), false};
    }
    dvec3 screenPosFixedDepth(screenPos.xy, 0.5);
    dvec3 positionFixedDepth_worldSpace = camera.mapScreenToWorld(screenPosFixedDepth);

    double scale = 1.0 / double(camera.viewSize().y) * 10.0;

    dmat4 modelMatrix = glm::scale(glm::translate(glm::dmat4(1), positionFixedDepth_worldSpace), dvec3(scale));
    return {modelMatrix, true};
}

std::tuple<double, double, bool> Manipulator::distanceFromArrow(dvec2 screenPos, const Camera& camera) {
    auto [manipulatorToWorld, isInScreen] = this->manipulatorToWorldMatrix(camera);
    if (!isInScreen) {
        return {0, 0, false};
    }

    dvec3 front = camera.mapScreenToCamera(vec3(screenPos, 0));
    dvec3 back = camera.mapScreenToCamera(vec3(screenPos, 1));

    dmat4 manipulatorToCamera = camera.worldToCameraMatrix() * manipulatorToWorld;

    dvec3 arrowXDirection = manipulatorToCamera[0].xyz;
    double scale = glm::length(arrowXDirection);
    dvec3 arrowCenter = manipulatorToCamera[3].xyz;

    Line mouseRay(front, back);
    Line arrowRay(arrowCenter, arrowCenter + arrowXDirection);
    LineLineDistance distance(mouseRay, arrowRay);

    return {distance.distance / scale, distance.t1, true};
}

} // namespace Lattice
