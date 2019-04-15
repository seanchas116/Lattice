#pragma once
#include "Tool.hpp"

namespace Lattice {
namespace Editor {
namespace MeshEditor {

class DrawTool : public Tool {
public:
    DrawTool(const SP<State::AppState>& appState, const SP<Document::MeshObject>& object) : Tool(appState, object) {}

    HitTestExclusion hitTestExclusion() const override;

    void mousePressTool(const EventTarget &target, const Viewport::MouseEvent &event) override;
    void mouseMoveTool(const EventTarget &target, const Viewport::MouseEvent &event) override;
    void mouseReleaseTool(const EventTarget &target, const Viewport::MouseEvent &event) override;
    void keyPressTool(QKeyEvent *event) override;

private:
    // draw
    std::vector<SP<Mesh::UVPoint>> _drawnUVPoints;
    Opt<SP<Mesh::UVPoint>> _previewUVPoint;
};

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
