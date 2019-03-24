#pragma once
#include "Tool.hpp"

namespace Lattice {
namespace Editor {
namespace MeshEditor {

class ExtrudeTool : public Tool {
public:
    ExtrudeTool(const SP<State::AppState>& appState, const SP<Document::MeshItem>& item) : Tool(appState, item) {}

    HitTestExclusion hitTestExclusion() const override;

    void mousePressEvent(const EventTarget &target, const Render::MouseEvent &event) override;
    void mouseMoveEvent(const EventTarget &target, const Render::MouseEvent &event) override;
    void mouseReleaseEvent(const EventTarget &target, const Render::MouseEvent &event) override;

private:
    bool _dragStarted {false};
    Mesh::MeshFragment _fragment;

    glm::dvec3 _initWorldPos {0};
    glm::dvec2 _initViewportPos {0};
    std::unordered_map<SP<Mesh::Vertex>, glm::dvec3> _initPositions;

    std::unordered_map<SP<Mesh::Vertex>, SP<Mesh::UVPoint>> _vertexToUV;
    std::unordered_map<SP<Mesh::UVPoint>, SP<Mesh::UVPoint>> _oldToNewUVPoints;

    bool _useGuide {false};
    glm::dvec3 _guideDirection {0};
};

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
