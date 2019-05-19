#pragma once
#include "Tool.hpp"

namespace Lattice {
namespace Editor {
namespace MeshEditor {

class DrawTool : public Tool {
public:
    DrawTool(const SP<Document::MeshObject>& object, const SP<Mesh::Mesh>& mesh) : Tool(object, mesh) {}

    HitTestExclusion hitTestExclusion() const override;

    void mousePressTool(const EventTarget &target, const Viewport::MouseEvent &event) override;
    void mouseMoveTool(const EventTarget &target, const Viewport::MouseEvent &event) override;
    void mouseReleaseTool(const EventTarget &target, const Viewport::MouseEvent &event) override;
    void keyPressTool(QKeyEvent *event) override;

private:
    // draw
    std::vector<Mesh::UVPointHandle> _drawnUVPoints;
    Opt<Mesh::UVPointHandle> _previewUVPoint;
};

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
