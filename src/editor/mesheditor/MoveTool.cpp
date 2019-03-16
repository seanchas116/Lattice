#include "MoveTool.hpp"
#include "../../document/Document.hpp"
#include "../../document/History.hpp"

using namespace glm;

namespace Lattice {
namespace Editor {
namespace MeshEditor {

void MoveTool::mousePress(const Tool::EventTarget &target, const Render::MouseEvent &event) {
    if (event.originalEvent->button() != Qt::LeftButton) {
        return;
    }
    std::unordered_set<SP<Mesh::Vertex>> vertices;

    if (target.vertex) {
        auto& vertex = *target.vertex;
        vertices = {vertex};
    } else if (target.edge) {
        auto& edge = *target.edge;
        vertices = {edge->vertices()[0], edge->vertices()[1]};
    } else if (target.face) {
        auto& face = *target.face;
        for (auto& v : face->vertices()) {
            vertices.insert(v);
        }
    }

    bool alreadySelected = true;
    auto oldSelection = appState()->document()->meshSelection();

    for (auto& v : vertices) {
        if (oldSelection.vertices.find(v) == oldSelection.vertices.end()) {
            alreadySelected = false;
        }
    }

    Document::MeshFragment selection;
    if (event.originalEvent->modifiers() & Qt::ShiftModifier) {
        selection = oldSelection;

        if (alreadySelected) {
            for (auto& v : vertices) {
                selection.vertices.erase(v);
            }
        } else {
            for (auto& v: vertices) {
                selection.vertices.insert(v);
            }
        }
    } else {
        selection.vertices = vertices;
    }
    _nextSelection = selection;

    auto& dragVertices = alreadySelected ? oldSelection.vertices : vertices;

    _dragged = true;
    _initPositions.clear();
    for (auto& v : dragVertices) {
        _initPositions[v] = v->position();
    }
    _initWorldPos = event.worldPos();
    _initScreenPos = event.screenPos;
    _dragStarted = false;
}

void MoveTool::mouseMove(const Tool::EventTarget &target, const Render::MouseEvent &event) {
    Q_UNUSED(target);

    if (!_dragged) {
        return;
    }

    dvec3 worldPos = event.worldPos();
    dvec3 offset = worldPos - _initWorldPos;

    if (!_dragStarted) {
        if (distance(_initScreenPos, event.screenPos) < appState()->preferences()->moveThreshold()) {
            return;
        }
        appState()->document()->history()->beginChange(tr("Move Vertex"));
        _dragStarted = true;
    }

    auto& mesh = item()->mesh();
    std::unordered_map<SP<Mesh::Vertex>, vec3> positions;
    for (auto& [v, initialPos] : _initPositions) {
        positions[v] = initialPos + offset;
    }
    mesh->setPositions(positions);
}

void MoveTool::mouseRelease(const Tool::EventTarget &target, const Render::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);
    _dragged = false;
    _initPositions.clear();
    if (!_dragStarted) {
        appState()->document()->setMeshSelection(_nextSelection);
    }
}

void MoveTool::hoverEnter(const Tool::EventTarget &target, const Render::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);
    // TOOD
}

void MoveTool::hoverLeave(const Tool::EventTarget &target) {
    Q_UNUSED(target);
    // TOOD
}

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
