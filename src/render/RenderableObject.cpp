#include "RenderableObject.hpp"

namespace Lattice {
namespace Render {

void RenderableObject::onUpdate() {
    emit updated();
}

} // namespace Renderer
} // namespace Lattice
