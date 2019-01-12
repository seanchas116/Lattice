#include "ScaleHandle.hpp"
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

ScaleHandle::ScaleHandle(int axis) :
    _axis(axis),
    _handleVAO(createHandleVAO()),
    _bodyVAO(createBodyVAO())
{
}

void ScaleHandle::draw(const SP<Render::Operations> &operations, const Camera &camera) {
    Coordinates coordinates(camera, _targetPosition);
    if (!coordinates.isInScreen){
        return;
    }

    dmat4 translate = glm::translate(dvec3(_length, 0, 0));
    operations->drawSolid.draw(_handleVAO, coordinates.manipulatorToWorld * Constants::swizzleTransforms[_axis] * translate, camera, vec3(0), Constants::colors[_axis]);

    _bodyVAO->vertexBuffer()->setVertices({{vec3(Constants::bodyBegin, 0, 0), {}, {}}, {vec3(_length, 0, 0), {}, {}}});
    operations->drawLine.draw(_bodyVAO, coordinates.manipulatorToWorld * Constants::swizzleTransforms[_axis], camera, Constants::bodyWidth, Constants::colors[_axis]);
}

std::optional<Render::HitResult> ScaleHandle::hitTest(dvec2 pos, const Camera &camera) const {
    Coordinates coordinates(camera, _targetPosition);
    if (!coordinates.isInScreen) {
        return {};
    }

    Ray mouseRay = camera.cameraMouseRay(pos);

    RayRayDistance mouseToArrowDistance(mouseRay, coordinates.arrowRaysInManipulatorSpace[_axis]);
    double distance = mouseToArrowDistance.distance / coordinates.scale;
    double tArrow = mouseToArrowDistance.t1;

    if (distance <= Constants::hitRadius) {
        if (Constants::bodyBegin <= tArrow && tArrow <= _length + Constants::translateHandleLength) {
            return {{mouseToArrowDistance.t0}};
        }
    }
    return {};
}

void ScaleHandle::mousePress(QMouseEvent *event, dvec2 pos, const Camera &camera, const Render::HitResult &hitResult) {
    Q_UNUSED(event); Q_UNUSED(hitResult);

    Coordinates coordinates(camera, _targetPosition);
    if (!coordinates.isInScreen) {
        return;
    }

    Ray mouseRay = camera.cameraMouseRay(pos);

    RayRayDistance mouseToAxisDistance(mouseRay, coordinates.axisRaysInCameraSpace[_axis]);
    double tAxis = mouseToAxisDistance.t1;

    _initialTargetPosition = _targetPosition;
    emit scaleStarted(tAxis);
}

void ScaleHandle::mouseMove(QMouseEvent *event, glm::dvec2 pos, const Camera &camera) {
    Q_UNUSED(event);

    Coordinates coordinates(camera, _initialTargetPosition);
    if (!coordinates.isInScreen) {
        return;
    }

    Ray mouseRay = camera.cameraMouseRay(pos);
    RayRayDistance mouseToAxisDistance(mouseRay, coordinates.axisRaysInCameraSpace[_axis]);
    double tAxis = mouseToAxisDistance.t1;

     emit scaleChanged(tAxis);
}

void ScaleHandle::mouseRelease(QMouseEvent *event, glm::dvec2 pos, const Camera &camera) {
    Q_UNUSED(event); Q_UNUSED(pos); Q_UNUSED(camera);
    emit scaleFinished();
}

SP<GL::VAO> ScaleHandle::createHandleVAO() {
    auto mesh = makeShared<Document::Mesh>();
    auto material = mesh->addMaterial();
    mesh->addCube(-dvec3(Constants::scaleHandleSize*0.5), +dvec3(Constants::scaleHandleSize*0.5), material);

    return MeshVAOGenerator(mesh).generateFaceVAOs().at(material);
}

SP<GL::LineVAO> ScaleHandle::createBodyVAO() {
    auto bodyVAO = makeShared<GL::LineVAO>();
    bodyVAO->vertexBuffer()->setVertices({{}, {}});
    bodyVAO->setLineStrips({{0, 1}});
    return bodyVAO;
}

}
} // namespace Editor
} // namespace Lattice