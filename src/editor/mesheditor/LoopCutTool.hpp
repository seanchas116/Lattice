#pragma once
#include "Tool.hpp"

namespace Lattice {
namespace Editor {
namespace MeshEditor {

class LoopCutTool : public Tool {
public:
    LoopCutTool(const SP<State::AppState>& appState, const SP<Document::MeshObject>& object, const SP<Mesh::Mesh>& mesh) : Tool(appState, object, mesh) {}

    void mousePressTool(const EventTarget &target, const Viewport::MouseEvent &event) override;
    void mouseMoveTool(const EventTarget &target, const Viewport::MouseEvent &event) override;
    void mouseReleaseTool(const EventTarget &target, const Viewport::MouseEvent &event) override;

private:
};

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
