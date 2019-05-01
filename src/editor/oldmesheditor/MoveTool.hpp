#pragma once
#include "Tool.hpp"
#include "BorderSelectTool.hpp"
#include "../../oldmesh/MeshFragment.hpp"

namespace Lattice {
namespace Editor {
namespace OldMeshEditor {

class MoveTool : public Tool {
public:
    MoveTool(const SP<State::AppState>& appState, const SP<Document::MeshObject>& object, const SP<Mesh::Mesh>& mesh);

    void mousePressTool(const EventTarget &target, const Viewport::MouseEvent &event) override;
    void mouseMoveTool(const EventTarget &target, const Viewport::MouseEvent &event) override;
    void mouseReleaseTool(const EventTarget &target, const Viewport::MouseEvent &event) override;

private:
    SP<BorderSelectTool> _borderSelectTool;
    OldMesh::MeshFragment _nextSelection;
    std::unordered_map<SP<OldMesh::Vertex>, glm::dvec3> _initPositions;
    glm::dvec3 _initObjectPos {0};
    glm::dvec2 _initViewportPos {0};
    bool _dragged {false};
    bool _dragStarted {false};
    bool _borderSelectMode {false};
};

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
