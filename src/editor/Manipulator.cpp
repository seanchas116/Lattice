#include "Manipulator.hpp"
#include "MeshPicker.hpp"
#include "MeshVAOGenerator.hpp"
#include "../render/Operations.hpp"
#include "../document/Mesh.hpp"
#include "../gl/VAO.hpp"
#include "../gl/LineVAO.hpp"
#include "../gl/PointVAO.hpp"
#include "../gl/VertexBuffer.hpp"
#include "../support/Debug.hpp"
#include "../support/Ray.hpp"
#include "../support/Distance.hpp"
#include <array>
#include <glm/gtx/transform.hpp>

using namespace glm;

namespace Lattice::Editor {

namespace {

constexpr double bodyBegin = 0.2;
constexpr double bodyWidth = 2.0;
constexpr double scaleHandleSize = 0.2;
constexpr double translateHandleLength = 0.4;
constexpr double translateHandleWidth = 0.2;
constexpr double hitRadius = 0.2;
constexpr double fixedDepth = 0.5;

const auto swizzleTransforms = std::array<dmat4, 3> {
        dmat4(1), // x
        dmat4(0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1), // y
        dmat4(0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1), // z
};

class ManipulatorCoordinates final {
public:
    ManipulatorCoordinates(const Camera& camera, glm::dvec3 targetPos) : targetPos(targetPos) {
        auto [screenPos, isInScreen] = camera.mapWorldToScreen(targetPos);
        this->isInScreen = isInScreen;
        if (!isInScreen) {
            return;
        }

        dvec3 screenPosFixedDepth(screenPos.xy, fixedDepth);
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

Manipulator::Manipulator() :
    // TODO: do not create meaningless default objects
    _translateHandleVAO(makeShared<GL::VAO>()),
    _scaleHandleVAO(makeShared<GL::VAO>()),
    _rotateHandleVAO(makeShared<GL::LineVAO>()),
    _rotateHandlePicker(makeShared<MeshPicker>(makeShared<Document::Mesh>())),
    _bodyVAO(makeShared<GL::LineVAO>()),
    _centerVAO(makeShared<GL::PointVAO>())
{
    initializeOpenGLFunctions();

    {
        auto mesh = makeShared<Document::Mesh>();
        auto material = mesh->addMaterial();
        mesh->addCone(dvec3(0), translateHandleWidth * 0.5, translateHandleLength, 8, 0, material);

        _translateHandleVAO = MeshVAOGenerator(mesh).generateFaceVAOs().at(material);
    }

    {
        auto mesh = makeShared<Document::Mesh>();
        auto material = mesh->addMaterial();
        mesh->addCube(-dvec3(scaleHandleSize*0.5), +dvec3(scaleHandleSize*0.5), material);

        _scaleHandleVAO = MeshVAOGenerator(mesh).generateFaceVAOs().at(material);
    }

    {
        auto mesh = makeShared<Document::Mesh>();
        auto material = mesh->addMaterial();
        mesh->addCircle(dvec3(0), 2.0, 64, Document::Mesh::CircleFill::None, 0, material);
        _rotateHandlePicker = makeShared<MeshPicker>(mesh);

        _rotateHandleVAO = MeshVAOGenerator(mesh).generateEdgeVAO();
    }

    {
        _bodyVAO = makeShared<GL::LineVAO>();
        _bodyVAO->vertexBuffer()->setVertices({{}, {}});
        _bodyVAO->setLineStrips({{0, 1}});
    }

    {
        _centerVAO = makeShared<GL::PointVAO>();
        GL::VertexBuffer::Vertex vertex{vec3(0), vec2(0), vec3(0)};
        _centerVAO->vertexBuffer()->setVertices({vertex});
    }
}

void Manipulator::draw(const SP<Render::Operations> &operations, const Camera &camera) {
    ManipulatorCoordinates coordinates(camera, _targetPosition);
    if (!coordinates.isInScreen){
        return;
    }

    auto colors = std::array<dvec3, 3> {
            dvec3(1, 0, 0), dvec3(0, 1, 0), dvec3(0, 0, 1)
    };

    _bodyVAO->vertexBuffer()->setVertices({{vec3(bodyBegin, 0, 0), {}, {}}, {vec3(bodyEnd(), 0, 0), {}, {}}});

    for (size_t i = 0; i < 3; ++i) {
        if (_isTranslateHandleVisible) {
            dmat4 translate = glm::translate(dvec3(translateHandleOffset(), 0, 0));
            operations->drawSolid.draw(_translateHandleVAO, coordinates.manipulatorToWorld * swizzleTransforms[i] * translate, camera, vec3(0), colors[i]);
        }
        if (_isScaleHandleVisible) {
            dmat4 translate = glm::translate(dvec3(scaleHandleOffset(), 0, 0));
            operations->drawSolid.draw(_scaleHandleVAO, coordinates.manipulatorToWorld * swizzleTransforms[i] * translate, camera, vec3(0), colors[i]);
        }
        if (_isRotateHandleVisible) {
            glClearDepthf(float((fixedDepth + 1.0) * 0.5));
            glClear(GL_DEPTH_BUFFER_BIT);
            operations->drawLine.draw(_rotateHandleVAO, coordinates.manipulatorToWorld * swizzleTransforms[i], camera, bodyWidth, colors[i]);
            glClearDepthf(1);
            glClear(GL_DEPTH_BUFFER_BIT);
        }

        if (_isTranslateHandleVisible || _isScaleHandleVisible) {
            operations->drawLine.draw(_bodyVAO, coordinates.manipulatorToWorld * swizzleTransforms[i], camera, bodyWidth, colors[i]);
        }
    }
    operations->drawCircle.draw(_centerVAO, coordinates.manipulatorToWorld, camera, 8, vec3(1));
}

std::pair<bool, double> Manipulator::mousePress(QMouseEvent *event, dvec2 pos, const Camera &camera) {
    Q_UNUSED(event)

    ManipulatorCoordinates coordinates(camera, _targetPosition);
    if (!coordinates.isInScreen) {
        return {false, 0};
    }

    Ray mouseRay = camera.cameraMouseRay(pos);

    for (int axis = 0; axis < 3; ++axis) {
        RayRayDistance mouseToArrowDistance(mouseRay, coordinates.arrowRaysInManipulatorSpace[axis]);
        RayRayDistance mouseToAxisDistance(mouseRay, coordinates.axisRaysInCameraSpace[axis]);

        double distance = mouseToArrowDistance.distance / coordinates.scale;
        double tArrow = mouseToArrowDistance.t1;
        double tAxis = mouseToAxisDistance.t1;

        if (distance <= hitRadius) {
            if (_isScaleHandleVisible && abs(tArrow - scaleHandleOffset()) <= scaleHandleSize) {
                _dragMode = DragMode::Scale;
                _initialDragValue = tAxis;
                _dragAxis = axis;
                emit scaleStarted();

                return {true, 0};
            }
            if (_isTranslateHandleVisible && bodyBegin <= tArrow && tArrow <= bodyEnd() + translateHandleLength) {
                _dragMode = DragMode::Translate;
                _initialDragValue = tAxis;
                _initialTargetPosition = _targetPosition;
                _dragAxis = axis;
                emit translateStarted();

                return {true, 0};
            }
        }

        if (_isRotateHandleVisible) {
            dmat4 rotateHandleMatrix = coordinates.manipulatorToCamera * swizzleTransforms[axis];
            dmat4 rotateHandleMatrixInverse = inverse(rotateHandleMatrix);
            auto rotateHandleRay = rotateHandleMatrixInverse * mouseRay;
            auto pickResult = _rotateHandlePicker->pickEdge(rotateHandleRay, 0.1);
            if (pickResult) {
                auto [edge, t] = *pickResult;
                auto [hitPos_screenSpace, isInScreen] = camera.mapCameraToScreen((rotateHandleMatrix * dvec4(rotateHandleRay.at(t), 1)).xyz);
                if (hitPos_screenSpace.z <= fixedDepth) {
                    dvec3 intersection = rotateHandleRay.whereXIsZero();
                    double angle = atan2(intersection.z, intersection.y);

                    _dragMode = DragMode::Rotate;
                    _initialDragValue = angle;
                    _initialTargetPosition = _targetPosition;
                    _dragAxis = axis;
                    emit rotateStarted();

                    return {true, 0};
                }
            }
        }
    }

    return {false, 0};
}

void Manipulator::mouseMove(QMouseEvent *event, dvec2 pos, const Camera &camera) {
    Q_UNUSED(event)

    if (_dragMode == DragMode::None) {
        return;
    }

    ManipulatorCoordinates coordinates(camera, _initialTargetPosition);
    if (!coordinates.isInScreen) {
        return;
    }

    Ray mouseRay = camera.cameraMouseRay(pos);
    RayRayDistance mouseToAxisDistance(mouseRay, coordinates.axisRaysInCameraSpace[_dragAxis]);
    double tAxis = mouseToAxisDistance.t1;

    switch (_dragMode) {
    case DragMode::Translate:{
        emit translateChanged(_dragAxis, tAxis - _initialDragValue);
        break;
    }
    case DragMode::Scale: {
        emit scaleChanged(_dragAxis, tAxis / _initialDragValue);
        break;
    }
    case DragMode::Rotate: {
        dmat4 rotateHandleMatrix = coordinates.manipulatorToCamera * swizzleTransforms[_dragAxis];
        dmat4 rotateHandleMatrixInverse = inverse(rotateHandleMatrix);
        auto rotateHandleRay = rotateHandleMatrixInverse * mouseRay;
        dvec3 intersection = rotateHandleRay.whereXIsZero();
        double angle = atan2(intersection.z, intersection.y);

        emit rotateChanged(_dragAxis, angle - _initialDragValue);
        break;
    }
    default:
        break;
    }
}

void Manipulator::mouseRelease(QMouseEvent *event, dvec2 pos, const Camera &camera) {
    Q_UNUSED(event)
    Q_UNUSED(pos)
    Q_UNUSED(camera)

    switch (_dragMode) {
    default:
        break;
    case DragMode::Translate:
        emit translateFinished();
        break;
    case DragMode::Scale:
        emit scaleFinished();
        break;
    case DragMode::Rotate:
        emit rotateFinished();
        break;
    }

    _dragMode = DragMode::None;
}

double Manipulator::translateHandleOffset() const {
    return _isScaleHandleVisible ? 2.2 : 2.0;
}

double Manipulator::scaleHandleOffset() const {
    return _isTranslateHandleVisible ? 1.8: 2.0;
}

double Manipulator::bodyEnd() const {
    if (_isScaleHandleVisible && _isTranslateHandleVisible) {
        return 2.2;
    }
    return 2.0;
}

} // namespace Lattice