#pragma once
#include "Tool.hpp"

namespace Lattice {
namespace Editor {
namespace MeshEditor {

class LoopCutTool : public Tool {
public:
    LoopCutTool(const SP<Document::MeshObject>& object, const SP<Mesh::Mesh>& mesh) : Tool(object, mesh) {}

    void mousePressTool(const EventTarget &target, const Viewport::MouseEvent &event) override;
    void mouseMoveTool(const EventTarget &target, const Viewport::MouseEvent &event) override;
    void mouseReleaseTool(const EventTarget &target, const Viewport::MouseEvent &event) override;

private:
};

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
