#pragma once
#include "Tool.hpp"

namespace Lattice {
namespace Editor {
namespace MeshEditor {

class LoopCutTool : public Tool {
public:
    LoopCutTool(const SP<State::AppState>& appState, const SP<Document::MeshObject>& object) : Tool(appState, object) {}

    void mousePressEvent(const EventTarget &target, const Render::MouseEvent &event) override;
    void mouseMoveEvent(const EventTarget &target, const Render::MouseEvent &event) override;
    void mouseReleaseEvent(const EventTarget &target, const Render::MouseEvent &event) override;

private:
};

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
