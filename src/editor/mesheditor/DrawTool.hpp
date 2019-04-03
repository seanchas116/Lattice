#pragma once
#include "Tool.hpp"

namespace Lattice {
namespace Editor {
namespace MeshEditor {

class DrawTool : public Tool {
public:
    DrawTool(const SP<State::AppState>& appState, const SP<Document::MeshObject>& object) : Tool(appState, object) {}

    HitTestExclusion hitTestExclusion() const override;

    void mousePressEvent(const EventTarget &target, const Render::MouseEvent &event) override;
    void mouseMoveEvent(const EventTarget &target, const Render::MouseEvent &event) override;
    void mouseReleaseEvent(const EventTarget &target, const Render::MouseEvent &event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    // draw
    std::vector<SP<Mesh::UVPoint>> _drawnUVPoints;
    Opt<SP<Mesh::UVPoint>> _previewUVPoint;
};

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
