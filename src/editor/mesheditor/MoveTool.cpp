#include "MoveTool.hpp"
#include "../../document/Document.hpp"
#include "../../document/History.hpp"
#include "../../mesh/Mesh.hpp"
#include "../../state/Preferences.hpp"

using namespace glm;

namespace Lattice {
namespace Editor {
namespace MeshEditor {

namespace {

// TODO: move to support
template <typename T>
bool set_includes(const std::unordered_set<T>& set, const std::unordered_set<T>& otherSet) {
    for (auto& value : otherSet) {
        if (set.find(value) == set.end()) {
            return false;
        }
    }
    return true;
}

}

MoveTool::MoveTool(const SP<State::MeshEditState> &meshEditState) :
    Tool(meshEditState),
    _borderSelectTool(makeShared<BorderSelectTool>(meshEditState))
{
    setChildRenderables({_borderSelectTool});
    _borderSelectTool->setVisible(false);
}

void MoveTool::mousePressTool(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    if (event.originalMouseEvent->button() != Qt::LeftButton) {
        return;
    }

    auto& mesh = *this->mesh();

    auto clickedVertices = target.vertices(mesh);
    if (clickedVertices.empty()) {
        _borderSelectMode = true;
        _borderSelectTool->setVisible(true);
        _borderSelectTool->mousePressTool(target, event);
        return;
    }

    auto oldSelection = mesh.selectedVertices() | ranges::to_<std::unordered_set<Mesh::VertexHandle>>();

    bool alreadySelected = !clickedVertices.empty() && set_includes(oldSelection, clickedVertices);

    std::unordered_set<Mesh::VertexHandle> selection;
    if (event.originalMouseEvent->modifiers() & Qt::ShiftModifier) {
        selection = oldSelection;

        if (alreadySelected) {
            for (auto& v : clickedVertices) {
                selection.erase(v);
            }
        } else {
            for (auto& v: clickedVertices) {
                selection.insert(v);
            }
        }
    } else {
        selection = clickedVertices;
    }
    _nextSelection = selection;

    auto& dragVertices = alreadySelected ? oldSelection : clickedVertices;

    _dragged = true;
    _initPositions.clear();
    for (auto& v : dragVertices) {
        _initPositions[v] = mesh.position(v);
    }
    _initObjectPos = (object()->location().matrixToModel() * dvec4(event.worldPos(), 1)).xyz;
    _initViewportPos = event.viewportPos;
    _dragStarted = false;
}

void MoveTool::mouseMoveTool(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    Q_UNUSED(target);

    if (_borderSelectMode) {
        _borderSelectTool->mouseMoveTool(target, event);
        return;
    }

    if (!_dragged) {
        return;
    }

    dvec3 objectPos = (object()->location().matrixToModel() * dvec4(event.worldPos(), 1)).xyz;
    dvec3 offset = objectPos - _initObjectPos;

    if (!_dragStarted) {
        if (distance(_initViewportPos, dvec2(event.viewportPos.xy)) < State::Preferences::shared().moveThreshold()) {
            return;
        }
        _dragStarted = true;
    }

    for (auto& [v, initialPos] : _initPositions) {
        mesh()->setPosition(v, initialPos + offset);
    }
    meshEditState()->notifyMeshChanged();
}

void MoveTool::mouseReleaseTool(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    if (_borderSelectMode) {
        _borderSelectTool->mouseReleaseTool(target, event);
        _borderSelectMode = false;
        _borderSelectTool->setVisible(false);
        return;
    }

    _dragged = false;
    _initPositions.clear();
    if (_dragStarted) {
        meshEditState()->commitMeshChanged(tr("Move Vertices"));
        emit finished();
    } else {
        mesh()->deselectAll();
        for (auto& v : _nextSelection) {
            mesh()->setSelected(v, true);
        }
        meshEditState()->notifyMeshChanged();
    }
}

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
