#include "RotateHandle.hpp"
#include "Constants.hpp"
#include "Coordinates.hpp"
#include "../OldMeshVAOGenerator.hpp"
#include "../../draw/Operations.hpp"
#include "../../oldmesh/Mesh.hpp"
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
    _handleVAO(OldMeshVAOGenerator(_handleMesh).generateEdgeVAO())
{
    initializeOpenGLFunctions();
}

void RotateHandle::draw(const SP<Draw::Operations> &operations, const SP<Camera> &camera) {
    Coordinates coordinates(camera, _targetPosition);
    if (!coordinates.isInViewport){
        return;
    }

    glClearDepthf(Constants::fixedDepth);
    glClear(GL_DEPTH_BUFFER_BIT);
    operations->drawLine.draw(_handleVAO, coordinates.manipulatorToWorld * Constants::swizzleTransforms[_axis], camera, Constants::bodyWidth, Constants::colors[_axis]);
    glClearDepthf(1);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void RotateHandle::drawPickables(const SP<Draw::Operations> &operations, const SP<Camera> &camera) {
    Coordinates coordinates(camera, _targetPosition);
    if (!coordinates.isInViewport){
        return;
    }

    glClearDepthf(Constants::fixedDepth);
    glClear(GL_DEPTH_BUFFER_BIT);
    operations->drawLine.draw(_handleVAO, coordinates.manipulatorToWorld * Constants::swizzleTransforms[_axis], camera, Constants::bodyWidth, toIDColor());
    glClearDepthf(1);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void RotateHandle::mousePressEvent(const Viewport::MouseEvent &event) {
    if (event.originalEvent->button() != Qt::LeftButton) {
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

void RotateHandle::contextMenuEvent(const Viewport::ContextMenuEvent &event) {
    emit onContextMenu(event);
}

SP<OldMesh::Mesh> RotateHandle::createMesh() {
    auto mesh = makeShared<OldMesh::Mesh>();
    auto material = mesh->addMaterial();
    mesh->addCircle(dvec3(0), 2.0, 64, OldMesh::Mesh::CircleFill::None, 0, material);
    return mesh;
}

}
} // namespace Editor
} // namespace Lattice
