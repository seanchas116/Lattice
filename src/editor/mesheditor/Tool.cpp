#include "Tool.hpp"

namespace Lattice {
namespace Editor {
namespace MeshEditor {

Tool::~Tool() {
}

Tool::HitTestExclusion Tool::hitTestExclusion() const {
    return {};
}

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
