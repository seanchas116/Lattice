#include "ArrowHandle.hpp"
#include "Constants.hpp"
#include "Coordinates.hpp"
#include "../MeshVAOGenerator.hpp"
#include "../../draw/Operations.hpp"
#include "../../mesh/Mesh.hpp"
#include "../../mesh/algorithm/ConeBuilder.hpp"
#include "../../mesh/algorithm/CubeBuilder.hpp"
#include "../../mesh/algorithm/BuildCylinder.hpp"
#include "../../gl/VAO.hpp"
#include "../../gl/VertexBuffer.hpp"
#include "../../support/Debug.hpp"
#include "../../support/Ray.hpp"
#include "../../support/Distance.hpp"
#include <QMouseEvent>

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

void ArrowHandle::draw(const Viewport::DrawEvent &event) {
    Coordinates coordinates(event.camera, _targetPosition);
    if (!coordinates.isInViewport){
        return;
    }

    dmat4 translate = glm::translate(dvec3(_length, 0, 0));
    event.operations->drawUnicolor.draw(_handleVAO, coordinates.manipulatorToWorld * Constants::swizzleTransforms[_axis] * translate, event.camera, _hovered ? Constants::hoverColors[_axis] : Constants::colors[_axis]);
    event.operations->drawLine.draw(_bodyVAO, coordinates.manipulatorToWorld * Constants::swizzleTransforms[_axis], event.camera, Constants::bodyWidth, _hovered ? Constants::hoverColors[_axis] : Constants::colors[_axis]);
}

void ArrowHandle::drawHitArea(const Viewport::DrawEvent &event) {
    Coordinates coordinates(event.camera, _targetPosition);
    if (!coordinates.isInViewport){
        return;
    }

    event.operations->drawUnicolor.draw(_bodyPickVAO, coordinates.manipulatorToWorld * Constants::swizzleTransforms[_axis], event.camera, toIDColor());
}

void ArrowHandle::mousePressEvent(const Viewport::MouseEvent &event) {
    if (event.originalMouseEvent->button() != Qt::LeftButton) {
        return;
    }

    Coordinates coordinates(event.camera, _targetPosition);
    if (!coordinates.isInViewport) {
        return;
    }

    RayRayDistanceSolver mouseToAxisDistanceSolver(event.camera.cameraMouseRay(event.viewportPos), coordinates.axisRaysInCameraSpace[_axis]);
    double tAxis = mouseToAxisDistanceSolver.t1;

    _initialTargetPosition = _targetPosition;
    _dragged = true;
    emit onDragBegin(tAxis);
}

void ArrowHandle::mouseMoveEvent(const Viewport::MouseEvent &event) {
    if (!_dragged) {
        return;
    }

    Coordinates coordinates(event.camera, _initialTargetPosition);
    if (!coordinates.isInViewport) {
        return;
    }

    RayRayDistanceSolver mouseToAxisDistanceSolver(event.camera.cameraMouseRay(event.viewportPos), coordinates.axisRaysInCameraSpace[_axis]);
    double tAxis = mouseToAxisDistanceSolver.t1;

    emit onDragMove(tAxis);
}

void ArrowHandle::mouseReleaseEvent(const Viewport::MouseEvent &event) {
    Q_UNUSED(event);
    _dragged = false;
    emit onDragEnd();
}

void ArrowHandle::contextMenuEvent(const Viewport::MouseEvent &event) {
    emit onContextMenu(event);
}

void ArrowHandle::hoverEnterEvent(const Viewport::MouseEvent &event) {
    Q_UNUSED(event);
    _hovered = true;
    emit updated();
}

void ArrowHandle::hoverLeaveEvent() {
    _hovered = false;
    emit updated();
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
    Mesh::Mesh mesh;
    Mesh::MaterialHandle material;
    if (_handleType == HandleType::Translate) {
        Mesh::ConeBuilder builder;
        builder.center = vec3(0);
        builder.radius = Constants::translateHandleWidth * 0.5;
        builder.height = Constants::translateHandleLength;
        builder.segmentCount = 8;
        builder.axis = 0;
        mesh = builder.build();
    } else {
        Mesh::CubeBuilder builder;
        builder.minPos = vec3(-Constants::scaleHandleSize*0.5);
        builder.maxPos = vec3(Constants::scaleHandleSize*0.5);
        mesh = builder.build();
    }
    return MeshVAOGenerator(mesh).generateFaceVAOs().at(material);
}

SP<GL::VAO> ArrowHandle::createBodyVAO(double length) {
    auto indexBuffer = makeShared<GL::IndexBuffer>();
    indexBuffer->setLineStrips({{0, 1}});
    auto vertexBuffer = makeShared<GL::VertexBuffer<Draw::PointLineVertex>>();
    vertexBuffer->setVertices({{vec3(Constants::bodyBegin, 0, 0), vec4(0), 1}, {vec3(length, 0, 0), vec4(0), 1}});
    auto bodyVAO = makeShared<GL::VAO>(vertexBuffer, indexBuffer);
    return bodyVAO;
}

SP<GL::VAO> ArrowHandle::createBodyPickVAO(double length) {
    Mesh::Mesh mesh;
    Mesh::MaterialHandle material;
    mesh = Mesh::BuildCylinder(vec3(Constants::bodyBegin, 0, 0), Constants::hitRadius, length - Constants::bodyBegin + Constants::translateHandleLength, 8, 0, material).perform();
    return MeshVAOGenerator(mesh).generateFaceVAOs().at(material);
}

}
} // namespace Editor
} // namespace Lattice
