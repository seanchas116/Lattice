#pragma once
#include "Tool.hpp"

namespace Lattice {
namespace Editor {
namespace MeshEditor {

class LoopCutTool : public Tool {
public:
    LoopCutTool(const SP<State::MeshEditState>& meshEditState) : Tool(meshEditState) {}

    void mousePressTool(const EventTarget &target, const Viewport::MouseEvent &event) override;
    void mouseMoveTool(const EventTarget &target, const Viewport::MouseEvent &event) override;
    void mouseReleaseTool(const EventTarget &target, const Viewport::MouseEvent &event) override;

private:
};

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
