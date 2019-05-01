#include "Tool.hpp"

namespace Lattice {
namespace Editor {
namespace MeshEditor {

Tool::~Tool() {
}

Tool::HitTestExclusion Tool::hitTestExclusion() const {
    return {};
}

void Tool::hoverEnterTool(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);
}

void Tool::hoverLeaveTool(const Tool::EventTarget &target) {
    Q_UNUSED(target);
}

void Tool::keyPressTool(QKeyEvent *event) {
    Q_UNUSED(event);
}

void Tool::keyReleaseTool(QKeyEvent *event) {
    Q_UNUSED(event);
}

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
