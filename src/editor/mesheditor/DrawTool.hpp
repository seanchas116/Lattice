#pragma once
#include "Tool.hpp"

namespace Lattice {
namespace Editor {
namespace MeshEditor {

class DrawTool : public Tool {
public:
    DrawTool(const SP<State::AppState>& appState, const SP<Document::MeshItem>& item) : Tool(appState, item) {}

    HitTestExclusion hitTestExclusion() const override;

    void mousePress(const EventTarget &target, const Render::MouseEvent &event) override;
    void mouseMove(const EventTarget &target, const Render::MouseEvent &event) override;
    void mouseRelease(const EventTarget &target, const Render::MouseEvent &event) override;
    void hoverEnter(const EventTarget &target, const Render::MouseEvent &event) override;
    void hoverLeave(const EventTarget &target) override;

private:

    // draw
    std::vector<SP<Mesh::UVPoint>> _drawnUVPoints;
    Opt<SP<Mesh::UVPoint>> lastDrawnPoint() const;
    Opt<SP<Mesh::Vertex>> lastDrawnVertex() const;
};

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
