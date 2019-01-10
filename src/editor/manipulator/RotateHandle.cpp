#include "RotateHandle.hpp"
#include "Constants.hpp"
#include "Coordinates.hpp"
#include "../MeshPicker.hpp"
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

RotateHandle::RotateHandle(int axis) :
    _axis(axis),
    _handleMesh(createMesh()),
    _handleMeshPicker(makeShared<MeshPicker>(_handleMesh)),
    _handleVAO(MeshVAOGenerator(_handleMesh).generateEdgeVAO())
{
    initializeOpenGLFunctions();
}

void RotateHandle::draw(const SP<Render::Operations> &operations, const Camera &camera) {
    Coordinates coordinates(camera, _targetPosition);
    if (!coordinates.isInScreen){
        return;
    }

    glClearDepthf(float((Constants::fixedDepth + 1.0) * 0.5));
    glClear(GL_DEPTH_BUFFER_BIT);
    operations->drawLine.draw(_handleVAO, coordinates.manipulatorToWorld * Constants::swizzleTransforms[_axis], camera, Constants::bodyWidth, Constants::colors[_axis]);
    glClearDepthf(1);
    glClear(GL_DEPTH_BUFFER_BIT);
}

std::optional<Render::HitResult> RotateHandle::hitTest(dvec2 pos, const Camera &camera) const {
    Coordinates coordinates(camera, _targetPosition);
    if (!coordinates.isInScreen) {
        return {};
    }

    Ray mouseRay = camera.cameraMouseRay(pos);

    dmat4 rotateHandleMatrix = coordinates.manipulatorToCamera * Constants::swizzleTransforms[_axis];
    dmat4 rotateHandleMatrixInverse = inverse(rotateHandleMatrix);
    auto rotateHandleRay = rotateHandleMatrixInverse * mouseRay;
    auto pickResult = _handleMeshPicker->pickEdge(rotateHandleRay, 0.1);
    if (!pickResult) {
        return {};
    }
    auto [edge, t] = *pickResult;
    auto [hitPos_screenSpace, isInScreen] = camera.mapCameraToScreen((rotateHandleMatrix * dvec4(rotateHandleRay.at(t), 1)).xyz);

    if (hitPos_screenSpace.z > Constants::fixedDepth) {
        return {};
    }
    return {{t}};
}

void RotateHandle::mousePress(QMouseEvent *event, dvec2 pos, const Camera &camera, const Render::HitResult &hitResult)  {
    Q_UNUSED(event)

    Coordinates coordinates(camera, _targetPosition);
    if (!coordinates.isInScreen) {
        return;
    }

    dmat4 rotateHandleMatrix = coordinates.manipulatorToCamera * Constants::swizzleTransforms[_axis];
    dmat4 rotateHandleMatrixInverse = inverse(rotateHandleMatrix);
    auto rotateHandleRay = rotateHandleMatrixInverse * camera.cameraMouseRay(pos);
    auto pickResult = _handleMeshPicker->pickEdge(rotateHandleRay, 0.1);

    dvec3 intersection = rotateHandleRay.whereXIsZero();
    double angle = atan2(intersection.z, intersection.y);

    _initialDragValue = angle;
    _initialTargetPosition = _targetPosition;
    emit rotateStarted();
}

void RotateHandle::mouseMove(QMouseEvent *event, glm::dvec2 pos, const Camera &camera) {
    Q_UNUSED(event);

    Coordinates coordinates(camera, _initialTargetPosition);
    if (!coordinates.isInScreen) {
        return;
    }

    Ray mouseRay = camera.cameraMouseRay(pos);

    dmat4 rotateHandleMatrix = coordinates.manipulatorToCamera * Constants::swizzleTransforms[_axis];
    dmat4 rotateHandleMatrixInverse = inverse(rotateHandleMatrix);
    auto rotateHandleRay = rotateHandleMatrixInverse * mouseRay;
    dvec3 intersection = rotateHandleRay.whereXIsZero();
    double angle = atan2(intersection.z, intersection.y);

    emit rotateChanged(angle - _initialDragValue);
}

void RotateHandle::mouseRelease(QMouseEvent *event, glm::dvec2 pos, const Camera &camera) {
    Q_UNUSED(event); Q_UNUSED(pos); Q_UNUSED(camera);
    emit rotateFinished();
}

SP<Document::Mesh> RotateHandle::createMesh() {
    auto mesh = makeShared<Document::Mesh>();
    auto material = mesh->addMaterial();
    mesh->addCircle(dvec3(0), 2.0, 64, Document::Mesh::CircleFill::None, 0, material);
    return mesh;
}

}
} // namespace Editor
} // namespace Lattice
