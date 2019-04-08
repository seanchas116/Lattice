#pragma once
#include "Tool.hpp"
#include "../../mesh/MeshFragment.hpp"

namespace Lattice {
namespace Editor {
namespace MeshEditor {

class BorderSelectTool : public Tool {
public:
    BorderSelectTool(const SP<State::AppState>& appState, const SP<Document::MeshObject>& object) : Tool(appState, object) {}

    void mousePressTool(const EventTarget &target, const Viewport::MouseEvent &event) override;
    void mouseMoveTool(const EventTarget &target, const Viewport::MouseEvent &event) override;
    void mouseReleaseTool(const EventTarget &target, const Viewport::MouseEvent &event) override;

private:
    glm::dvec2 _initViewportPos {0};
    bool _dragged {false};
};

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
