#pragma once
#include "Tool.hpp"

namespace Lattice {
namespace Editor {
namespace MeshEditor {

class BorderSelectTool : public Tool {
  public:
    BorderSelectTool(const SP<State::MeshEditState> &meshEditState) : Tool(meshEditState) {}

    void mousePressTool(const EventTarget &target, const Viewport::MouseEvent &event) override;
    void mouseMoveTool(const EventTarget &target, const Viewport::MouseEvent &event) override;
    void mouseReleaseTool(const EventTarget &target, const Viewport::MouseEvent &event) override;

    void draw2D(const Viewport::Draw2DEvent &event) override;

  private:
    struct VertexWithScreenPos {
        meshlib::VertexHandle vertex;
        glm::dvec2 screenPos;
    };

    std::vector<VertexWithScreenPos> _vertices;
    glm::dvec2 _initViewportPos{0};
    glm::dvec2 _currentViewportPos{0};
    Viewport::Viewport *_draggedViewport = nullptr;
};

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
