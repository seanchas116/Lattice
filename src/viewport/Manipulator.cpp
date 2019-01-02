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

constexpr double bodyLength = 2.0;
constexpr double bodyWidth = 2.0;
constexpr double headLength = 0.4;
constexpr double headWidth = 0.2;
constexpr uint32_t headSegmentCount = 16;
constexpr double hitRadius = 0.2;

class ManipulatorMetrics {
public:
    ManipulatorMetrics(const Camera& camera, glm::dvec3 targetPos) : camera(camera), targetPos(targetPos) {
        auto [screenPos, isInScreen] = camera.mapWorldToScreen(targetPos);
        this->isInScreen = isInScreen;
        if (!isInScreen) {
            return;
        }

        dvec3 screenPosFixedDepth(screenPos.xy, 0.5);
        dvec3 positionFixedDepth_worldSpace = camera.mapScreenToWorld(screenPosFixedDepth);

        scale = 1.0 / double(camera.viewSize().y) * 10.0;

        dmat4 worldToCamera = camera.worldToCameraMatrix();
        manipulatorToWorld = glm::scale(glm::translate(glm::dmat4(1), positionFixedDepth_worldSpace), dvec3(scale));
        manipulatorToCamera = worldToCamera * manipulatorToWorld;

        dmat4 targetToCamera = worldToCamera * glm::translate(glm::dmat4(1), targetPos);

        for (int axis = 0; axis < 3; ++axis) {
            arrowLinesInManipulatorSpace[axis] = Line(manipulatorToCamera[3].xyz, manipulatorToCamera[3].xyz + manipulatorToCamera[axis].xyz);
            axisLinesInCameraSpace[axis] = Line(targetToCamera[3].xyz, targetToCamera[3].xyz + targetToCamera[axis].xyz);
        }
    }

    Camera camera;
    dvec3 targetPos;
    bool isInScreen;
    dmat4 manipulatorToWorld;
    dmat4 manipulatorToCamera;
    double scale;
    std::array<Line, 3> arrowLinesInManipulatorSpace;
    std::array<Line, 3> axisLinesInCameraSpace;
};

}

Manipulator::Manipulator() {
    {
        _headVAO = std::make_shared<VAO>();

        std::vector<VertexBuffer::Vertex> vertices;
        std::vector<VAO::Triangle> triangles;

        double headRadius = headWidth * 0.5;

        for (uint32_t i = 0; i < headSegmentCount; ++i) {
            double angle = 2.0 * M_PI / headSegmentCount * i;
            dvec3 pos(bodyLength, cos(angle) * headRadius, sin(angle) * headRadius);
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
    ManipulatorMetrics metrics(camera, _targetPosition);
    if (!metrics.isInScreen){
        return;
    }

    auto transforms = std::array<dmat4, 3> {
            dmat4(1), // x
            dmat4(0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1), // y
            dmat4(0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1), // z
    };
    auto colors = std::array<dvec3, 3> {
            dvec3(1, 0, 0), dvec3(0, 1, 0), dvec3(0, 0, 1)
    };

    for (size_t i = 0; i < 3; ++i) {
        operations->drawSolid.draw(_headVAO, metrics.manipulatorToWorld * transforms[i], camera, vec3(0), colors[i]);
        operations->drawLine.draw(_bodyVAO, metrics.manipulatorToWorld * transforms[i], camera, bodyWidth, colors[i]);
    }
}

bool Manipulator::mousePress(QMouseEvent *event, dvec2 pos, const Camera &camera) {
    Q_UNUSED(event)

    ManipulatorMetrics metrics(camera, _targetPosition);
    if (!metrics.isInScreen) {
        return false;
    }

    Line mouseRay = camera.cameraMouseRay(pos);

    for (int axis = 0; axis < 3; ++axis) {
        LineLineDistance mouseToArrowDistance(mouseRay, metrics.arrowLinesInManipulatorSpace[axis]);
        LineLineDistance mouseToAxisDistance(mouseRay, metrics.axisLinesInCameraSpace[axis]);

        double distance = mouseToArrowDistance.distance / metrics.scale;
        double tArrow = mouseToArrowDistance.t1;
        double tAxis = mouseToAxisDistance.t1;

        if (0 <= tArrow && tArrow <= bodyLength + headLength && distance <= hitRadius) {
            _isDragging = true;
            _initialDragValue = dvec3(0);
            _initialDragValue[axis] = tAxis;
            _initialTargetPosition = _targetPosition;
            _dragAxis = axis;
            qDebug() << tAxis;
            emit onDragStart();

            return true;
        }
    }

    return false;
}

bool Manipulator::mouseMove(QMouseEvent *event, dvec2 pos, const Camera &camera) {
    Q_UNUSED(event)

    if (!_isDragging) {
        return false;
    }

    ManipulatorMetrics metrics(camera, _initialTargetPosition);
    if (!metrics.isInScreen) {
        return false;
    }

    Line mouseRay = camera.cameraMouseRay(pos);
    LineLineDistance mouseToAxisDistance(mouseRay, metrics.axisLinesInCameraSpace[_dragAxis]);
    double tAxis = mouseToAxisDistance.t1;

    qDebug() << tAxis;
    dvec3 currentValue(0);
    currentValue[_dragAxis] = tAxis;
    emit onDrag(currentValue - _initialDragValue);
    return true;
}

bool Manipulator::mouseRelease(QMouseEvent *event, dvec2 pos, const Camera &camera) {
    Q_UNUSED(event)
    Q_UNUSED(pos)
    Q_UNUSED(camera)

    if (!_isDragging) {
        return false;
    }
    _isDragging = false;
    emit onDragEnd();
    return true;
}

std::pair<dmat4, bool> Manipulator::manipulatorToWorldMatrix(vec3 targetPos, const Camera &camera) const {
    auto [screenPos, isInScreen] = camera.mapWorldToScreen(targetPos);
    if (!isInScreen){
        return {mat4(), false};
    }
    dvec3 screenPosFixedDepth(screenPos.xy, 0.5);
    dvec3 positionFixedDepth_worldSpace = camera.mapScreenToWorld(screenPosFixedDepth);

    double scale = 1.0 / double(camera.viewSize().y) * 10.0;

    dmat4 modelMatrix = glm::scale(glm::translate(glm::dmat4(1), positionFixedDepth_worldSpace), dvec3(scale));
    return {modelMatrix, true};
}

std::tuple<double, double, double, bool> Manipulator::distanceFromArrow(int axis, dvec3 targetPos, dvec2 screenPos, const Camera& camera) {
    // TODO: refactor

    auto [manipulatorToWorld, isInScreen] = this->manipulatorToWorldMatrix(targetPos, camera);
    if (!isInScreen) {
        return {0, 0, 0, false};
    }

    dmat4 manipulatorToCamera = camera.worldToCameraMatrix() * manipulatorToWorld;

    dvec3 arrowXDirection = manipulatorToCamera[axis].xyz;
    double scale = glm::length(arrowXDirection);
    dvec3 arrowCenter = manipulatorToCamera[3].xyz;

    Line mouseRay = camera.cameraMouseRay(screenPos);
    Line arrowRay(arrowCenter, arrowCenter + arrowXDirection);

    dvec3 targetPositionCamera = (camera.worldToCameraMatrix() * vec4(targetPos, 1)).xyz;
    dvec3 axisDirection = camera.worldToCameraMatrix()[axis].xyz;
    Line axisRay(targetPositionCamera, targetPositionCamera + axisDirection);
    qDebug() << axisDirection;

    LineLineDistance mouseToArrayDistance(mouseRay, arrowRay);
    LineLineDistance mouseToAxisDistance(mouseRay, axisRay);

    return {mouseToArrayDistance.distance / scale, mouseToArrayDistance.t1, mouseToAxisDistance.t1, true};
}

} // namespace Lattice
