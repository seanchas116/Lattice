#pragma once
#include "BorderSelectTool.hpp"
#include "Tool.hpp"

namespace Lattice {
namespace Editor {
namespace MeshEditor {

class MoveTool : public Tool {
  public:
    MoveTool(const SP<State::MeshEditState> &meshEditState);

    void mousePressTool(const EventTarget &target, const Viewport::MouseEvent &event) override;
    void mouseMoveTool(const EventTarget &target, const Viewport::MouseEvent &event) override;
    void mouseReleaseTool(const EventTarget &target, const Viewport::MouseEvent &event) override;

  private:
    SP<BorderSelectTool> _borderSelectTool;
    std::unordered_set<meshlib::VertexHandle> _nextSelection;
    std::unordered_map<meshlib::VertexHandle, glm::dvec3> _initPositions;
    glm::dvec3 _initObjectPos{0};
    glm::dvec2 _initViewportPos{0};
    bool _dragged{false};
    bool _dragStarted{false};
    bool _borderSelectMode{false};
};

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
