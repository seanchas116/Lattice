#pragma once
#include "Tool.hpp"

namespace Lattice {
namespace Editor {
namespace MeshEditor {

class ExtrudeTool : public Tool {
public:
    ExtrudeTool(const SP<State::AppState>& appState, const SP<Document::MeshObject>& object, const SP<Mesh::Mesh>& mesh) : Tool(appState, object, mesh) {}

    HitTestExclusion hitTestExclusion() const override;

    void mousePressTool(const EventTarget &target, const Viewport::MouseEvent &event) override;
    void mouseMoveTool(const EventTarget &target, const Viewport::MouseEvent &event) override;
    void mouseReleaseTool(const EventTarget &target, const Viewport::MouseEvent &event) override;

private:
    bool _dragStarted {false};
    std::unordered_set<Mesh::VertexHandle> _vertices;

    glm::dvec3 _initWorldPos {0};
    glm::dvec2 _initViewportPos {0};
    std::unordered_map<Mesh::VertexHandle, glm::dvec3> _initPositions;

    std::unordered_map<Mesh::VertexHandle, Mesh::UVPointHandle> _vertexToUV;
    std::unordered_map<Mesh::UVPointHandle, Mesh::UVPointHandle> _oldToNewUVPoints;

    bool _useGuide {false};
    glm::dvec3 _guideDirection {0};
};

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
