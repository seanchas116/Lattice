#pragma once
#include "../render/Renderable.hpp"

namespace Lattice {
namespace Editor {

class Background : public Render::Renderable, protected QOpenGLExtraFunctions {
public:
    Background();
    void draw(const SP<Render::Operations> &operations, const Camera &camera) override;
};

} // namespace Editor
} // namespace Lattice
