#include "ArrowHandle.hpp"
#include "Constants.hpp"
#include "Coordinates.hpp"
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

ArrowHandle::ArrowHandle(int axis, HandleType handleType) :
    _axis(axis),
    _handleType(handleType),
    _handleVAO(createHandleVAO()),
    _bodyVAO(createBodyVAO(_length)),
    _bodyPickVAO(createBodyPickVAO(_length))
{
}

void ArrowHandle::draw(const SP<Render::Operations> &operations, const SP<Camera> &camera) {
    Coordinates coordinates(camera, _targetPosition);
    if (!coordinates.isInScreen){
        return;
    }

    dmat4 translate = glm::translate(dvec3(_length, 0, 0));
    operations->drawSolid.draw(_handleVAO, coordinates.manipulatorToWorld * Constants::swizzleTransforms[_axis] * translate, camera, vec3(0), _hovered ? Constants::hoverColors[_axis] : Constants::colors[_axis]);
    operations->drawLine.draw(_bodyVAO, coordinates.manipulatorToWorld * Constants::swizzleTransforms[_axis], camera, Constants::bodyWidth, _hovered ? Constants::hoverColors[_axis] : Constants::colors[_axis]);
}

void ArrowHandle::drawPickables(const SP<Render::Operations> &operations, const SP<Camera> &camera) {
    Coordinates coordinates(camera, _targetPosition);
    if (!coordinates.isInScreen){
        return;
    }

    operations->drawUnicolor.draw(_bodyPickVAO, coordinates.manipulatorToWorld * Constants::swizzleTransforms[_axis], camera, toIDColor());
}

void ArrowHandle::mousePress(const Render::MouseEvent &event) {
    Coordinates coordinates(event.camera, _targetPosition);
    if (!coordinates.isInScreen) {
        return;
    }

    RayRayDistance mouseToAxisDistance(event.camera->cameraMouseRay(event.screenPos), coordinates.axisRaysInCameraSpace[_axis]);
    double tAxis = mouseToAxisDistance.t1;

    _initialTargetPosition = _targetPosition;
    emit onBegin(tAxis);
}

void ArrowHandle::mouseMove(const Render::MouseEvent &event) {
    Coordinates coordinates(event.camera, _initialTargetPosition);
    if (!coordinates.isInScreen) {
        return;
    }

    RayRayDistance mouseToAxisDistance(event.camera->cameraMouseRay(event.screenPos), coordinates.axisRaysInCameraSpace[_axis]);
    double tAxis = mouseToAxisDistance.t1;

    emit onChange(tAxis);
}

void ArrowHandle::mouseRelease(const Render::MouseEvent &event) {
    Q_UNUSED(event);
    emit onEnd();
}

void ArrowHandle::hoverEnter(const Render::MouseEvent &event) {
    Q_UNUSED(event);
    _hovered = true;
    update();
}

void ArrowHandle::hoverLeave() {
    _hovered = false;
    update();
}

void ArrowHandle::setLength(double length) {
    if (_length == length) {
        return;
    }
    _length = length;
    _bodyVAO = createBodyVAO(length);
    _bodyPickVAO = createBodyPickVAO(length);
}

SP<GL::VAO> ArrowHandle::createHandleVAO() {
    auto mesh = makeShared<Document::Mesh>();
    auto material = mesh->addMaterial();
    if (_handleType == HandleType::Translate) {
        mesh->addCone(dvec3(0), Constants::translateHandleWidth * 0.5, Constants::translateHandleLength, 8, 0, material);
    } else {
        mesh->addCube(-dvec3(Constants::scaleHandleSize*0.5), +dvec3(Constants::scaleHandleSize*0.5), material);
    }

    return MeshVAOGenerator(mesh).generateFaceVAOs().at(material);
}

SP<GL::VAO> ArrowHandle::createBodyVAO(double length) {
    auto indexBuffer = makeShared<GL::IndexBuffer>();
    indexBuffer->setLineStrips({{0, 1}});
    auto vertexBuffer = makeShared<GL::VertexBuffer<GL::Vertex>>();
    vertexBuffer->setVertices({{vec3(Constants::bodyBegin, 0, 0), {}, {}}, {vec3(length, 0, 0), {}, {}}});
    auto bodyVAO = makeShared<GL::VAO>(vertexBuffer, indexBuffer);
    return bodyVAO;
}

SP<GL::VAO> ArrowHandle::createBodyPickVAO(double length) {
    auto mesh = makeShared<Document::Mesh>();
    auto material = mesh->addMaterial();
    mesh->addCylinder(dvec3(Constants::bodyBegin, 0, 0), Constants::hitRadius, length - Constants::bodyBegin + Constants::translateHandleLength, 8, 0, material);
    return MeshVAOGenerator(mesh).generateFaceVAOs().at(material);
}

}
} // namespace Editor
} // namespace Lattice
