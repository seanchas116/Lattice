#pragma once
#include "Tool.hpp"
#include "../../mesh/MeshFragment.hpp"

namespace Lattice {
namespace Editor {
namespace MeshEditor {

class BorderSelectTool : public Tool {
public:
    BorderSelectTool(const SP<State::AppState>& appState, const SP<Document::MeshObject>& object) : Tool(appState, object) {}

    void mousePressEvent(const EventTarget &target, const Viewport::MouseEvent &event) override;
    void mouseMoveEvent(const EventTarget &target, const Viewport::MouseEvent &event) override;
    void mouseReleaseEvent(const EventTarget &target, const Viewport::MouseEvent &event) override;

    void drawOverlay(QPainter *painter, const QSize &viewportSize) override;

private:
    glm::dvec2 _initViewportPos {0};
    glm::dvec2 _currentViewportPos {0};
    bool _dragged {false};
};

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
