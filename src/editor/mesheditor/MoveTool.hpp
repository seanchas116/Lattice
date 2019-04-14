#pragma once
#include "Tool.hpp"
#include "BorderSelectTool.hpp"
#include "../../mesh/MeshFragment.hpp"

namespace Lattice {
namespace Editor {
namespace MeshEditor {

class MoveTool : public Tool {
public:
    MoveTool(const SP<State::AppState>& appState, const SP<Document::MeshObject>& object);

    void mousePressEvent(const EventTarget &target, const Viewport::MouseEvent &event) override;
    void mouseMoveEvent(const EventTarget &target, const Viewport::MouseEvent &event) override;
    void mouseReleaseEvent(const EventTarget &target, const Viewport::MouseEvent &event) override;
    void drawOverlay(QPainter *painter, const QSize &viewportSize) override;

private:
    BorderSelectTool _borderSelectTool;
    Mesh::MeshFragment _nextSelection;
    std::unordered_map<SP<Mesh::Vertex>, glm::dvec3> _initPositions;
    glm::dvec3 _initObjectPos {0};
    glm::dvec2 _initViewportPos {0};
    bool _dragged {false};
    bool _dragStarted {false};
    bool _borderSelectMode {false};
};

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
