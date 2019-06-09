#include "RotateHandle.hpp"
#include "Constants.hpp"
#include "Coordinates.hpp"
#include "../MeshVAOGenerator.hpp"
#include "../../draw/Operations.hpp"
#include "../../mesh/Mesh.hpp"
#include "../../mesh/algorithm/BuildCircle.hpp"
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
    _handleVAO(MeshVAOGenerator(*_handleMesh).generateEdgeVAO())
{
}

void RotateHandle::draw(const SP<Draw::Operations> &operations, const SP<OldCamera> &camera) {
    Coordinates coordinates(camera, _targetPosition);
    if (!coordinates.isInViewport){
        return;
    }

    operations->clear.clearDepth(Constants::fixedDepth);
    operations->drawLine.draw(_handleVAO, coordinates.manipulatorToWorld * Constants::swizzleTransforms[_axis], camera, Constants::bodyWidth, Constants::colors[_axis]);
    operations->clear.clearDepth(1);
}

void RotateHandle::drawHitArea(const SP<Draw::Operations> &operations, const SP<OldCamera> &camera) {
    Coordinates coordinates(camera, _targetPosition);
    if (!coordinates.isInViewport){
        return;
    }

    operations->clear.clearDepth(Constants::fixedDepth);
    operations->drawLine.draw(_handleVAO, coordinates.manipulatorToWorld * Constants::swizzleTransforms[_axis], camera, Constants::bodyWidth, toIDColor());
    operations->clear.clearDepth(1);
}

void RotateHandle::mousePressEvent(const Viewport::MouseEvent &event) {
    if (event.originalMouseEvent->button() != Qt::LeftButton) {
        return;
    }

    Coordinates coordinates(event.camera, _targetPosition);
    if (!coordinates.isInViewport) {
        return;
    }

    dmat4 rotateHandleMatrix = coordinates.manipulatorToCamera * Constants::swizzleTransforms[_axis];
    dmat4 rotateHandleMatrixInverse = inverse(rotateHandleMatrix);
    auto rotateHandleRay = rotateHandleMatrixInverse * event.camera->cameraMouseRay(event.viewportPos);

    dvec3 intersection = rotateHandleRay.whereXIsZero();
    double angle = atan2(intersection.z, intersection.y);

    _initialTargetPosition = _targetPosition;
    _dragged = true;
    emit onDragBegin(angle);
}

void RotateHandle::mouseMoveEvent(const Viewport::MouseEvent &event) {
    if (!_dragged) {
        return;
    }

    Coordinates coordinates(event.camera, _initialTargetPosition);
    if (!coordinates.isInViewport) {
        return;
    }

    dmat4 rotateHandleMatrix = coordinates.manipulatorToCamera * Constants::swizzleTransforms[_axis];
    dmat4 rotateHandleMatrixInverse = inverse(rotateHandleMatrix);
    auto rotateHandleRay = rotateHandleMatrixInverse * event.camera->cameraMouseRay(event.viewportPos);
    dvec3 intersection = rotateHandleRay.whereXIsZero();
    double angle = atan2(intersection.z, intersection.y);

    emit onDragMove(angle);
}

void RotateHandle::mouseReleaseEvent(const Viewport::MouseEvent &event) {
    Q_UNUSED(event);
    _dragged = false;
    emit onDragEnd();
}

void RotateHandle::contextMenuEvent(const Viewport::MouseEvent &event) {
    emit onContextMenu(event);
}

SP<Mesh::Mesh> RotateHandle::createMesh() {
    auto mesh = makeShared<Mesh::Mesh>();
    *mesh = Mesh::BuildCircle(vec3(0), 2.f, 64, 0, Mesh::MaterialHandle()).perform();
    return mesh;
}

}
} // namespace Editor
} // namespace Lattice
