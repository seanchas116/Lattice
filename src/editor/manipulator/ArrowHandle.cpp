#include "ArrowHandle.hpp"
#include "Constants.hpp"
#include "Coordinates.hpp"
#include "../MeshVAOGenerator.hpp"
#include "../../render/Operations.hpp"
#include "../../mesh/Mesh.hpp"
#include "../../gl/VAO.hpp"
#include "../../gl/VertexBuffer.hpp"
#include "../../support/Debug.hpp"
#include "../../support/Ray.hpp"
#include "../../support/Distance.hpp"
#include "../../drawable/LinesDrawable.hpp"

using namespace glm;

namespace Lattice {
namespace Editor {
namespace Manipulator {

ArrowHandle::ArrowHandle(int axis, HandleType handleType) :
    _axis(axis),
    _handleType(handleType),
    _handleVAO(createHandleVAO()),
    _bodyDrawable(createBodyDrawable(_length)),
    _bodyPickVAO(createBodyPickVAO(_length))
{
}

void ArrowHandle::draw(const SP<Render::Operations> &operations, const SP<Camera> &camera) {
    Coordinates coordinates(camera, _targetPosition);
    if (!coordinates.isInViewport){
        return;
    }

    dmat4 translate = glm::translate(dvec3(_length, 0, 0));
    operations->drawSolid.draw(_handleVAO, coordinates.manipulatorToWorld * Constants::swizzleTransforms[_axis] * translate, camera, vec3(0), _hovered ? Constants::hoverColors[_axis] : Constants::colors[_axis]);

    _bodyDrawable->setColor(_hovered ? Constants::hoverColors[_axis] : Constants::colors[_axis]);
    _bodyDrawable->setUseVertexColor(false);
    _bodyDrawable->draw(operations->singletonBag, coordinates.manipulatorToWorld * Constants::swizzleTransforms[_axis], camera);
}

void ArrowHandle::drawPickables(const SP<Render::Operations> &operations, const SP<Camera> &camera) {
    Coordinates coordinates(camera, _targetPosition);
    if (!coordinates.isInViewport){
        return;
    }

    operations->drawUnicolor.draw(_bodyPickVAO, coordinates.manipulatorToWorld * Constants::swizzleTransforms[_axis], camera, toIDColor());
}

void ArrowHandle::mousePressEvent(const Render::MouseEvent &event) {
    if (event.originalEvent->button() != Qt::LeftButton) {
        return;
    }

    Coordinates coordinates(event.camera, _targetPosition);
    if (!coordinates.isInViewport) {
        return;
    }

    RayRayDistanceSolver mouseToAxisDistanceSolver(event.camera->cameraMouseRay(event.viewportPos), coordinates.axisRaysInCameraSpace[_axis]);
    double tAxis = mouseToAxisDistanceSolver.t1;

    _initialTargetPosition = _targetPosition;
    _dragged = true;
    emit onDragBegin(tAxis);
}

void ArrowHandle::mouseMoveEvent(const Render::MouseEvent &event) {
    if (!_dragged) {
        return;
    }

    Coordinates coordinates(event.camera, _initialTargetPosition);
    if (!coordinates.isInViewport) {
        return;
    }

    RayRayDistanceSolver mouseToAxisDistanceSolver(event.camera->cameraMouseRay(event.viewportPos), coordinates.axisRaysInCameraSpace[_axis]);
    double tAxis = mouseToAxisDistanceSolver.t1;

    emit onDragMove(tAxis);
}

void ArrowHandle::mouseReleaseEvent(const Render::MouseEvent &event) {
    Q_UNUSED(event);
    _dragged = false;
    emit onDragEnd();
}

void ArrowHandle::contextMenuEvent(const Render::ContextMenuEvent &event) {
    emit onContextMenu(event);
}

void ArrowHandle::hoverEnterEvent(const Render::MouseEvent &event) {
    Q_UNUSED(event);
    _hovered = true;
    update();
}

void ArrowHandle::hoverLeaveEvent() {
    _hovered = false;
    update();
}

void ArrowHandle::setLength(double length) {
    if (_length == length) {
        return;
    }
    _length = length;
    _bodyDrawable = createBodyDrawable(length);
    _bodyPickVAO = createBodyPickVAO(length);
}

SP<GL::VAO> ArrowHandle::createHandleVAO() {
    auto mesh = makeShared<Mesh::Mesh>();
    auto material = mesh->addMaterial();
    if (_handleType == HandleType::Translate) {
        mesh->addCone(dvec3(0), Constants::translateHandleWidth * 0.5, Constants::translateHandleLength, 8, 0, material);
    } else {
        mesh->addCube(-dvec3(Constants::scaleHandleSize*0.5), +dvec3(Constants::scaleHandleSize*0.5), material);
    }

    return MeshVAOGenerator(mesh).generateFaceVAOs().at(material);
}

SP<Drawable::LinesDrawable> ArrowHandle::createBodyDrawable(double length) {
    auto drawable = makeShared<Drawable::LinesDrawable>();
    drawable->setWidth(Constants::bodyWidth);
    drawable->setPoints({{vec3(Constants::bodyBegin, 0, 0), {}}, {vec3(length, 0, 0), {}}});
    drawable->setLineStrips({{0, 1}});
    return drawable;
}

SP<GL::VAO> ArrowHandle::createBodyPickVAO(double length) {
    auto mesh = makeShared<Mesh::Mesh>();
    auto material = mesh->addMaterial();
    mesh->addCylinder(dvec3(Constants::bodyBegin, 0, 0), Constants::hitRadius, length - Constants::bodyBegin + Constants::translateHandleLength, 8, 0, material);
    return MeshVAOGenerator(mesh).generateFaceVAOs().at(material);
}

}
} // namespace Editor
} // namespace Lattice
