#pragma once
#include "Tool.hpp"

namespace Lattice {
namespace Editor {
namespace MeshEditor {

class DrawTool : public Tool {
  public:
    DrawTool(const SP<State::MeshEditState> &meshEditState) : Tool(meshEditState) {}

    HitTestExclusion hitTestExclusion() const override;

    void mousePressTool(const EventTarget &target, const Viewport::MouseEvent &event) override;
    void mouseMoveTool(const EventTarget &target, const Viewport::MouseEvent &event) override;
    void mouseReleaseTool(const EventTarget &target, const Viewport::MouseEvent &event) override;
    void keyPressTool(QKeyEvent *event) override;

  private:
    // draw
    std::vector<meshlib::UVPointHandle> _drawnUVPoints;
    Opt<meshlib::UVPointHandle> _previewUVPoint;
};

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
