#pragma once
#include "Tool.hpp"

namespace Lattice {
namespace Editor {
namespace OldMeshEditor {

class ExtrudeTool : public Tool {
public:
    ExtrudeTool(const SP<State::AppState>& appState, const SP<Document::MeshObject>& object, const SP<Mesh::Mesh>& mesh) : Tool(appState, object, mesh) {}

    HitTestExclusion hitTestExclusion() const override;

    void mousePressTool(const EventTarget &target, const Viewport::MouseEvent &event) override;
    void mouseMoveTool(const EventTarget &target, const Viewport::MouseEvent &event) override;
    void mouseReleaseTool(const EventTarget &target, const Viewport::MouseEvent &event) override;

private:
    bool _dragStarted {false};
    OldMesh::MeshFragment _fragment;

    glm::dvec3 _initWorldPos {0};
    glm::dvec2 _initViewportPos {0};
    std::unordered_map<SP<OldMesh::Vertex>, glm::dvec3> _initPositions;

    std::unordered_map<SP<OldMesh::Vertex>, SP<OldMesh::UVPoint>> _vertexToUV;
    std::unordered_map<SP<OldMesh::UVPoint>, SP<OldMesh::UVPoint>> _oldToNewUVPoints;

    bool _useGuide {false};
    glm::dvec3 _guideDirection {0};
};

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
