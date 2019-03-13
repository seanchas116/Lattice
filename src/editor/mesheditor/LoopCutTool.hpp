#pragma once
#include "Tool.hpp"

namespace Lattice {
namespace Editor {
namespace MeshEditor {

class LoopCutTool : public Tool {
public:
    LoopCutTool(const SP<UI::AppState>& appState, const SP<Document::MeshItem>& item) : Tool(appState, item) {}

    void mousePress(const EventTarget &target, const Render::MouseEvent &event) override;
    void mouseMove(const EventTarget &target, const Render::MouseEvent &event) override;
    void mouseRelease(const EventTarget &target, const Render::MouseEvent &event) override;
    void hoverEnter(const EventTarget &target, const Render::MouseEvent &event) override;
    void hoverLeave(const EventTarget &target) override;

private:
};

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
