#include "MeshRenderer.hpp"
#include "MeshVAOGenerator.hpp"
#include "../state/AppState.hpp"
#include "../gl/VAO.hpp"
#include "../document/Document.hpp"
#include "../document/History.hpp"
#include "../document/MeshObject.hpp"
#include "../state/Preferences.hpp"
#include "../support/Debug.hpp"
#include <QMouseEvent>
#include <QMenu>

using namespace glm;

namespace Lattice {
namespace Editor {

MeshRenderer::MeshRenderer(const SP<State::AppState>& appState, const SP<Document::MeshObject> &object) :
    _appState(appState),
    _object(object)
{
    updateVAOs();
    connect(object.get(), &Document::MeshObject::meshChanged, this, &MeshRenderer::handleMeshUpdated);
    connect(object.get(), &Document::MeshObject::subdivSettingsChanged, this, &MeshRenderer::handleMeshUpdated);
}

void MeshRenderer::draw(const Viewport::DrawEvent &event) {
    updateVAOs();
    for (auto& [materialID, vao] : _faceVAOs) {
        auto material = _object->materials().at(materialID.index).toDrawMaterial();
        event.operations->drawMaterial.draw(vao, _object->location().matrixToWorld(), event.camera, material);
    }
}

void MeshRenderer::drawHitArea(const Viewport::DrawEvent &event) {
    updateVAOs();
    for (auto& [material, vao] : _faceVAOs) {
        event.operations->drawUnicolor.draw(vao, _object->location().matrixToWorld(), event.camera, toIDColor());
    }
}

void MeshRenderer::mousePressEvent(const Viewport::MouseEvent &event) {
    switch (event.originalMouseEvent->button()) {
    case Qt::RightButton: {
        QMenu contextMenu;
        contextMenu.addAction(tr("Delete"), _appState.get(), &State::AppState::deleteObjects);
        contextMenu.exec(event.originalMouseEvent->globalPos());
        return;
    }
    case Qt::LeftButton: {
        glm::dvec3 worldPos = event.worldPos();
        auto [viewportDragPos, isInViewport] = event.camera.mapWorldToViewport(worldPos);
        if (!isInViewport) {
            return;
        }

        _dragged = true;
        _dragInitLocation = _object->location();
        _dragInitWorldPos = worldPos;
        _dragInitViewportPos = event.viewportPos;
        _dragStarted = false;

        _appState->document()->selectObject(_object, event.originalMouseEvent->modifiers() & Qt::ShiftModifier);
        return;
    }
    default:
        return;
    }
}

void MeshRenderer::mouseMoveEvent(const Viewport::MouseEvent &event) {
    if (!_dragged) {
        return;
    }

    auto newWorldPos = event.worldPos();
    auto newLocation = _dragInitLocation;
    newLocation.position += newWorldPos - _dragInitWorldPos;

    if (!_dragStarted) {
        if (glm::distance(_dragInitViewportPos, dvec2(event.viewportPos.xy)) < State::Preferences::shared().moveThreshold()) {
            return;
        }
        _appState->document()->history()->beginChange(tr("Move Object"));
        _dragStarted = true;
    }
    _object->setLocation(newLocation);
}

void MeshRenderer::mouseReleaseEvent(const Viewport::MouseEvent &event) {
    Q_UNUSED(event);
    _dragged = false;
}

void MeshRenderer::mouseDoubleClickEvent(const Viewport::MouseEvent &event) {
    Q_UNUSED(event);
    _appState->startEditing(_object);
}

void MeshRenderer::handleMeshUpdated() {
    _isVAOsDirty = true;
    emit updated();
}

void MeshRenderer::updateVAOs() {
    if (!_isVAOsDirty) {
        return;
    }

    MeshVAOGenerator generator(_object->mesh());
    if (_object->subdivSettings().isEnabled) {
        _faceVAOs = generator.generateSubdivFaceVAOs(_object->subdivSettings().segmentCount);
    } else {
        _faceVAOs = generator.generateFaceVAOs();
    }

    _isVAOsDirty = false;
}

}
}
