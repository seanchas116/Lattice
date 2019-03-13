#pragma once
#include "Tool.hpp"

namespace Lattice {
namespace Editor {
namespace MeshEditor {

class MoveTool : public Tool {
public:
    MoveTool(const SP<UI::AppState>& appState, const SP<Document::MeshItem>& item) : Tool(appState, item) {}

    void mousePress(const EventTarget &target, const Render::MouseEvent &event) override;
    void mouseMove(const EventTarget &target, const Render::MouseEvent &event) override;
    void mouseRelease(const EventTarget &target, const Render::MouseEvent &event) override;
    void hoverEnter(const EventTarget &target, const Render::MouseEvent &event) override;
    void hoverLeave(const EventTarget &target) override;

private:
    std::unordered_map<SP<Mesh::Vertex>, glm::dvec3> _initPositions;
    glm::dvec3 _initWorldPos {0};
    bool _dragged {false};
    bool _dragStarted {false};
};

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
