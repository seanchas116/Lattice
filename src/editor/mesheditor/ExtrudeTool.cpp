#include "ExtrudeTool.hpp"
#include "../../document/Document.hpp"
#include "../../document/History.hpp"

using namespace glm;

namespace Lattice {
namespace Editor {
namespace MeshEditor {

Tool::HitTestExclusion ExtrudeTool::hitTestExclusion() const {
    return {};
}

void ExtrudeTool::mousePress(const Tool::EventTarget &target, const Render::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);
}

void ExtrudeTool::mouseMove(const Tool::EventTarget &target, const Render::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);
}

void ExtrudeTool::mouseRelease(const Tool::EventTarget &target, const Render::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);
}

void ExtrudeTool::hoverEnter(const Tool::EventTarget &target, const Render::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);
    // TOOD
}

void ExtrudeTool::hoverLeave(const Tool::EventTarget &target) {
    Q_UNUSED(target);
    // TOOD
}

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
