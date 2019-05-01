#pragma once
#include "Tool.hpp"

namespace Lattice {
namespace Editor {
namespace MeshEditor {

class BorderSelectTool : public Tool {
public:
    BorderSelectTool(const SP<State::AppState>& appState, const SP<Document::MeshObject>& object, const SP<Mesh::Mesh>& mesh) : Tool(appState, object, mesh) {}

    void mousePressTool(const EventTarget &target, const Viewport::MouseEvent &event) override;
    void mouseMoveTool(const EventTarget &target, const Viewport::MouseEvent &event) override;
    void mouseReleaseTool(const EventTarget &target, const Viewport::MouseEvent &event) override;

    void draw2D(QPainter *painter, const QSize &viewportSize) override;

private:
    struct VertexWithScreenPos {
        Mesh::VertexHandle vertex;
        glm::dvec2 screenPos;
    };

    std::vector<VertexWithScreenPos> _vertices;
    glm::dvec2 _initViewportPos {0};
    glm::dvec2 _currentViewportPos {0};
    bool _dragged {false};
};

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
