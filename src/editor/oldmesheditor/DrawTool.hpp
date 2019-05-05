#pragma once
#include "Tool.hpp"

namespace Lattice {
namespace Editor {
namespace OldMeshEditor {

class DrawTool : public Tool {
public:
    DrawTool(const SP<State::AppState>& appState, const SP<Document::MeshObject>& object, const SP<Mesh::Mesh>& mesh) : Tool(appState, object, mesh) {}

    HitTestExclusion hitTestExclusion() const override;

    void mousePressTool(const EventTarget &target, const Viewport::MouseEvent &event) override;
    void mouseMoveTool(const EventTarget &target, const Viewport::MouseEvent &event) override;
    void mouseReleaseTool(const EventTarget &target, const Viewport::MouseEvent &event) override;
    void keyPressTool(QKeyEvent *event) override;

private:
    // draw
    std::vector<SP<OldMesh::UVPoint>> _drawnUVPoints;
    Opt<SP<OldMesh::UVPoint>> _previewUVPoint;
};

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
