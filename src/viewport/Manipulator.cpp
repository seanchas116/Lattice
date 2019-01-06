#include "Manipulator.hpp"
#include "Operations.hpp"
#include "MeshVAOGenerator.hpp"
#include "../document/Mesh.hpp"
#include "../gl/VAO.hpp"
#include "../gl/LineVAO.hpp"
#include "../gl/PointVAO.hpp"
#include "../gl/VertexBuffer.hpp"
#include "../support/Debug.hpp"
#include "../support/Ray.hpp"
#include <array>
#include <glm/gtx/transform.hpp>

using namespace glm;

namespace Lattice::Viewport {

namespace {

constexpr double bodyBegin = 0.2;
constexpr double bodyEnd = 2.0;
constexpr double bodyWidth = 2.0;
constexpr double scaleHandleOffset = 1.6;
constexpr double scaleHandleSize = 0.2;
constexpr double translateHandleLength = 0.4;
constexpr double translateHandleWidth = 0.2;
constexpr double hitRadius = 0.2;

class ManipulatorMetrics final {
public:
    ManipulatorMetrics(const Camera& camera, glm::dvec3 targetPos) : targetPos(targetPos) {
        auto [screenPos, isInScreen] = camera.mapWorldToScreen(targetPos);
        this->isInScreen = isInScreen;
        if (!isInScreen) {
            return;
        }

        dvec3 screenPosFixedDepth(screenPos.xy, 0.5);
        dvec3 positionFixedDepth_worldSpace = camera.mapScreenToWorld(screenPosFixedDepth);

        scale = 1.0 / double(camera.viewSize().y) * 20.0;

        dmat4 worldToCamera = camera.worldToCameraMatrix();
        manipulatorToWorld = glm::scale(glm::translate(glm::dmat4(1), positionFixedDepth_worldSpace), dvec3(scale));
        manipulatorToCamera = worldToCamera * manipulatorToWorld;

        dmat4 targetToCamera = worldToCamera * glm::translate(targetPos);

        for (int axis = 0; axis < 3; ++axis) {
            arrowRaysInManipulatorSpace[axis] = {manipulatorToCamera[3].xyz, manipulatorToCamera[axis].xyz};
            axisRaysInCameraSpace[axis] = {targetToCamera[3].xyz, targetToCamera[axis].xyz};
        }
    }

    dvec3 targetPos;
    bool isInScreen;
    dmat4 manipulatorToWorld;
    dmat4 manipulatorToCamera;
    double scale;
    std::array<Ray<double>, 3> arrowRaysInManipulatorSpace;
    std::array<Ray<double>, 3> axisRaysInCameraSpace;
};

}

Manipulator::Manipulator() {
    {
        auto mesh = std::make_shared<Document::Mesh>();
        auto material = mesh->addMaterial();
        mesh->addCone(dvec3(bodyEnd, 0, 0), translateHandleWidth * 0.5, translateHandleLength, 8, 0, material);

        _translateHandleVAO = MeshVAOGenerator(mesh).generateFaceVAOs().at(material);
    }

    {
        auto mesh = std::make_shared<Document::Mesh>();
        auto material = mesh->addMaterial();
        dvec3 scaleHandleCenter(scaleHandleOffset, 0, 0);
        mesh->addCube(scaleHandleCenter - dvec3(scaleHandleSize*0.5), scaleHandleCenter + dvec3(scaleHandleSize*0.5), material);

        _scaleHandleVAO = MeshVAOGenerator(mesh).generateFaceVAOs().at(material);
    }

    {
        _bodyVAO = std::make_shared<GL::LineVAO>();

        std::vector<GL::VertexBuffer::Vertex> bodyVertices;
        std::vector<std::vector<uint32_t>> bodyLineStrips;

        bodyVertices.push_back({vec3(bodyBegin, 0, 0), {}, {}});
        bodyVertices.push_back({vec3(bodyEnd, 0, 0), {}, {}});
        bodyLineStrips = { {0, 1} };

        _bodyVAO->vertexBuffer()->setVertices(bodyVertices);
        _bodyVAO->setLineStrips(bodyLineStrips);
    }

    {
        _centerVAO = std::make_shared<GL::PointVAO>();
        GL::VertexBuffer::Vertex vertex{vec3(0), vec2(0), vec3(0)};
        _centerVAO->vertexBuffer()->setVertices({vertex});
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
        if (_isTranslateHandleVisible) {
            operations->drawSolid.draw(_translateHandleVAO, metrics.manipulatorToWorld * transforms[i], camera, vec3(0), colors[i]);
        }
        if (_isScaleHandleVisible) {
            operations->drawSolid.draw(_scaleHandleVAO, metrics.manipulatorToWorld * transforms[i], camera, vec3(0), colors[i]);
        }
        operations->drawLine.draw(_bodyVAO, metrics.manipulatorToWorld * transforms[i], camera, bodyWidth, colors[i]);
    }
    operations->drawCircle.draw(_centerVAO, metrics.manipulatorToWorld, camera, 8, vec3(1));
}

bool Manipulator::mousePress(QMouseEvent *event, dvec2 pos, const Camera &camera) {
    Q_UNUSED(event)

    ManipulatorMetrics metrics(camera, _targetPosition);
    if (!metrics.isInScreen) {
        return false;
    }

    Ray mouseRay = camera.cameraMouseRay(pos);

    for (int axis = 0; axis < 3; ++axis) {
        RayRayDistance mouseToArrowDistance(mouseRay, metrics.arrowRaysInManipulatorSpace[axis]);
        RayRayDistance mouseToAxisDistance(mouseRay, metrics.axisRaysInCameraSpace[axis]);

        double distance = mouseToArrowDistance.distance / metrics.scale;
        double tArrow = mouseToArrowDistance.t1;
        double tAxis = mouseToAxisDistance.t1;

        if (bodyBegin <= tArrow && tArrow <= bodyEnd + translateHandleLength && distance <= hitRadius) {
            _isDragging = true;
            _initialDragValue = dvec3(0);
            _initialDragValue[axis] = tAxis;
            _initialTargetPosition = _targetPosition;
            _dragAxis = axis;
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

    Ray mouseRay = camera.cameraMouseRay(pos);
    RayRayDistance mouseToAxisDistance(mouseRay, metrics.axisRaysInCameraSpace[_dragAxis]);
    double tAxis = mouseToAxisDistance.t1;

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

} // namespace Lattice
