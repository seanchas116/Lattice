#pragma once
#include "Tool.hpp"

namespace Lattice {
namespace Editor {
namespace MeshEditor {

class MoveTool : public Tool {
public:
    void mousePress(const EventTarget &target, const Render::MouseEvent &event) override;
    void mouseMove(const EventTarget &target, const Render::MouseEvent &event) override;
    void mouseRelease(const EventTarget &target, const Render::MouseEvent &event) override;
    void hoverEnter(const EventTarget &target, const Render::MouseEvent &event) override;
    void hoverLeave(const EventTarget &target) override;
};

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
