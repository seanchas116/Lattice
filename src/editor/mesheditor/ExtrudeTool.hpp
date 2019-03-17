#pragma once
#include "Tool.hpp"

namespace Lattice {
namespace Editor {
namespace MeshEditor {

class ExtrudeTool : public Tool {
public:
    ExtrudeTool(const SP<State::AppState>& appState, const SP<Document::MeshItem>& item) : Tool(appState, item) {}

    HitTestExclusion hitTestExclusion() const override;

    void mousePress(const EventTarget &target, const Render::MouseEvent &event) override;
    void mouseMove(const EventTarget &target, const Render::MouseEvent &event) override;
    void mouseRelease(const EventTarget &target, const Render::MouseEvent &event) override;
    void hoverEnter(const EventTarget &target, const Render::MouseEvent &event) override;
    void hoverLeave(const EventTarget &target) override;

private:
    bool _dragStarted {false};
    Mesh::MeshFragment _fragment;

    std::unordered_map<SP<Mesh::Vertex>, glm::dvec3> _initPositions;
    std::unordered_map<SP<Mesh::Vertex>, SP<Mesh::UVPoint>> _vertexToUV;
    std::unordered_map<SP<Mesh::UVPoint>, SP<Mesh::UVPoint>> _oldToNewUVPoints;
    glm::dvec3 _initWorldPos {0};
    glm::dvec2 _initScreenPos {0};
    bool _useGuide {false};
    glm::dvec3 _guideDirection {0};
};

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
