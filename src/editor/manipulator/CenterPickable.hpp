#pragma once
#include "../../render/RenderableObject.hpp"

namespace Lattice {
namespace Editor {
namespace Manipulator {

class CenterPickable : public Render::RenderableObject {
    Q_OBJECT
public:
    CenterPickable();
};

} // namespace Manipulator
} // namespace Editor
} // namespace Lattice
