#pragma once
#include "Tool.hpp"

namespace Lattice {
namespace Editor {
namespace MeshEditor {

class ExtrudeTool : public Tool {
public:
    ExtrudeTool(const SP<State::MeshEditState>& meshEditState) : Tool(meshEditState) {}

    HitTestExclusion hitTestExclusion() const override;

    void mousePressTool(const EventTarget &target, const Viewport::MouseEvent &event) override;
    void mouseMoveTool(const EventTarget &target, const Viewport::MouseEvent &event) override;
    void mouseReleaseTool(const EventTarget &target, const Viewport::MouseEvent &event) override;

private:
    bool _dragStarted {false};
    std::vector<Mesh::VertexHandle> _vertices;
    std::vector<Mesh::VertexHandle> _newVertices;

    glm::dvec3 _initWorldPos {0};
    glm::dvec2 _initViewportPos {0};

    bool _useGuide {false};
    glm::dvec3 _guideDirection {0};
};

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
