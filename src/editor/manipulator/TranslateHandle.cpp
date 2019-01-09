#include "TranslateHandle.hpp"
#include "Constants.hpp"
#include "Coordinates.hpp"
#include "../MeshVAOGenerator.hpp"
#include "../../render/Operations.hpp"
#include "../../document/Mesh.hpp"
#include "../../gl/VAO.hpp"
#include "../../gl/LineVAO.hpp"
#include "../../gl/PointVAO.hpp"
#include "../../gl/VertexBuffer.hpp"
#include "../../support/Debug.hpp"
#include "../../support/Ray.hpp"
#include "../../support/Distance.hpp"

using namespace glm;

namespace Lattice {
namespace Editor {
namespace Manipulator {

TranslateHandle::TranslateHandle(int axis) :
    _axis(axis),
    _handleVAO(createHandleVAO()),
    _bodyVAO(createBodyVAO())
{
}

void TranslateHandle::draw(const SP<Render::Operations> &operations, const Camera &camera) {
    Coordinates coordinates(camera, _targetPosition);
    if (!coordinates.isInScreen){
        return;
    }

    const auto colors = std::array<dvec3, 3> {
            dvec3(1, 0, 0), dvec3(0, 1, 0), dvec3(0, 0, 1)
    };

    const auto swizzleTransforms = std::array<dmat4, 3> {
            dmat4(1), // x
            dmat4(0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1), // y
            dmat4(0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1), // z
    };

    dmat4 translate = glm::translate(dvec3(_length, 0, 0));
    operations->drawSolid.draw(_handleVAO, coordinates.manipulatorToWorld * swizzleTransforms[_axis] * translate, camera, vec3(0), colors[_axis]);

    _bodyVAO->vertexBuffer()->setVertices({{vec3(Constants::bodyBegin, 0, 0), {}, {}}, {vec3(_length, 0, 0), {}, {}}});
    operations->drawLine.draw(_bodyVAO, coordinates.manipulatorToWorld * swizzleTransforms[_axis], camera, Constants::bodyWidth, colors[_axis]);
}

std::pair<bool, double> TranslateHandle::mousePress(QMouseEvent *event, glm::dvec2 pos, const Camera &camera) {
    Q_UNUSED(event)

    Coordinates coordinates(camera, _targetPosition);
    if (!coordinates.isInScreen) {
        return {false, 0};
    }

    Ray mouseRay = camera.cameraMouseRay(pos);

    RayRayDistance mouseToArrowDistance(mouseRay, coordinates.arrowRaysInManipulatorSpace[_axis]);
    RayRayDistance mouseToAxisDistance(mouseRay, coordinates.axisRaysInCameraSpace[_axis]);

    double distance = mouseToArrowDistance.distance / coordinates.scale;
    double tArrow = mouseToArrowDistance.t1;
    double tAxis = mouseToAxisDistance.t1;

    if (distance <= Constants::hitRadius) {
        if (Constants::bodyBegin <= tArrow && tArrow <= _length + Constants::translateHandleLength) {
            _initialDragValue = tAxis;
            _initialTargetPosition = _targetPosition;
            emit translateStarted();

            return {true, 0};
        }
    }
    return {false, 0};
}

void TranslateHandle::mouseMove(QMouseEvent *event, glm::dvec2 pos, const Camera &camera) {
    Q_UNUSED(event);

    Coordinates coordinates(camera, _initialTargetPosition);
    if (!coordinates.isInScreen) {
        return;
    }

    Ray mouseRay = camera.cameraMouseRay(pos);
    RayRayDistance mouseToAxisDistance(mouseRay, coordinates.axisRaysInCameraSpace[_axis]);
    double tAxis = mouseToAxisDistance.t1;

    emit translateChanged(tAxis - _initialDragValue);
}

void TranslateHandle::mouseRelease(QMouseEvent *event, glm::dvec2 pos, const Camera &camera) {
    Q_UNUSED(event); Q_UNUSED(pos); Q_UNUSED(camera);
    emit translateFinished();
}

SP<GL::VAO> TranslateHandle::createHandleVAO() {
    auto mesh = makeShared<Document::Mesh>();
    auto material = mesh->addMaterial();
    mesh->addCone(dvec3(0), Constants::translateHandleWidth * 0.5, Constants::translateHandleLength, 8, 0, material);

    return MeshVAOGenerator(mesh).generateFaceVAOs().at(material);
}

SP<GL::LineVAO> TranslateHandle::createBodyVAO() {
    auto bodyVAO = makeShared<GL::LineVAO>();
    bodyVAO->vertexBuffer()->setVertices({{}, {}});
    bodyVAO->setLineStrips({{0, 1}});
    return bodyVAO;
}

}
} // namespace Editor
} // namespace Lattice
