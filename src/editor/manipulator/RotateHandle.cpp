#include "RotateHandle.hpp"
#include "Constants.hpp"
#include "Coordinates.hpp"
#include "../MeshPicker.hpp"
#include "../MeshVAOGenerator.hpp"
#include "../../render/Operations.hpp"
#include "../../document/Mesh.hpp"
#include "../../gl/VAO.hpp"
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

void RotateHandle::draw(const SP<Render::Operations> &operations, const SP<Camera> &camera) {
    Coordinates coordinates(camera, _targetPosition);
    if (!coordinates.isInScreen){
        return;
    }

    glClearDepthf(Constants::fixedDepth);
    glClear(GL_DEPTH_BUFFER_BIT);
    operations->drawLine.draw(_handleVAO, coordinates.manipulatorToWorld * Constants::swizzleTransforms[_axis], camera, Constants::bodyWidth, Constants::colors[_axis]);
    glClearDepthf(1);
    glClear(GL_DEPTH_BUFFER_BIT);
}

Opt<Render::HitResult> RotateHandle::hitTest(dvec2 pos, const SP<Camera> &camera) const {
    Coordinates coordinates(camera, _targetPosition);
    if (!coordinates.isInScreen) {
        return {};
    }

    auto pickResult = _handleMeshPicker->pickEdge(coordinates.manipulatorToWorld * Constants::swizzleTransforms[_axis], camera, pos, 6);
    if (!pickResult) {
        return {};
    }
    auto [edge, depth] = *pickResult;
    if (depth > Constants::fixedDepth) {
        return {};
    }
    Render::HitResult result;
    result.depth = depth;
    return result;
}

void RotateHandle::mousePress(const Render::MouseEvent &event) {
    Coordinates coordinates(event.camera, _targetPosition);
    if (!coordinates.isInScreen) {
        return;
    }

    dmat4 rotateHandleMatrix = coordinates.manipulatorToCamera * Constants::swizzleTransforms[_axis];
    dmat4 rotateHandleMatrixInverse = inverse(rotateHandleMatrix);
    auto rotateHandleRay = rotateHandleMatrixInverse * event.camera->cameraMouseRay(event.screenPos);

    dvec3 intersection = rotateHandleRay.whereXIsZero();
    double angle = atan2(intersection.z, intersection.y);

    _initialTargetPosition = _targetPosition;
    emit onBegin(angle);
}

void RotateHandle::mouseMove(const Render::MouseEvent &event) {
    Q_UNUSED(event);

    Coordinates coordinates(event.camera, _initialTargetPosition);
    if (!coordinates.isInScreen) {
        return;
    }

    dmat4 rotateHandleMatrix = coordinates.manipulatorToCamera * Constants::swizzleTransforms[_axis];
    dmat4 rotateHandleMatrixInverse = inverse(rotateHandleMatrix);
    auto rotateHandleRay = rotateHandleMatrixInverse * event.camera->cameraMouseRay(event.screenPos);
    dvec3 intersection = rotateHandleRay.whereXIsZero();
    double angle = atan2(intersection.z, intersection.y);

    emit onChange(angle);
}

void RotateHandle::mouseRelease(const Render::MouseEvent &event) {
    Q_UNUSED(event);
    emit onEnd();
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
