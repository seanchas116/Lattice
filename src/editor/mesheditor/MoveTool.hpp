#pragma once
#include "Tool.hpp"
#include "../../mesh/MeshFragment.hpp"

namespace Lattice {
namespace Editor {
namespace MeshEditor {

class MoveTool : public Tool {
public:
    MoveTool(const SP<State::AppState>& appState, const SP<Document::MeshItem>& item) : Tool(appState, item) {}

    void mousePressEvent(const EventTarget &target, const Render::MouseEvent &event) override;
    void mouseMoveEvent(const EventTarget &target, const Render::MouseEvent &event) override;
    void mouseReleaseEvent(const EventTarget &target, const Render::MouseEvent &event) override;

private:
    Mesh::MeshFragment _nextSelection;
    std::unordered_map<SP<Mesh::Vertex>, glm::dvec3> _initPositions;
    glm::dvec3 _initItemPos {0};
    glm::dvec2 _initViewportPos {0};
    bool _dragged {false};
    bool _dragStarted {false};
};

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
