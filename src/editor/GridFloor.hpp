#pragma once

#include "../support/Shorthands.hpp"
#include "../render/RenderableObject.hpp"
#include <glm/glm.hpp>

namespace Lattice {
class Camera;

namespace GL {
struct Vertex;
template <typename T> class VertexBuffer;
class IndexBuffer;
}

namespace Drawable {
class LinesDrawable;
}

namespace Editor {

class GridFloor final : public Render::RenderableObject {
    Q_OBJECT
public:
    GridFloor();

    void draw(const SP<Render::Operations>& operations, const SP<Camera>& camera) override;

private:
    SP<Drawable::LinesDrawable> _drawable;
    SP<Drawable::LinesDrawable> _xAxisDrawable;
    SP<Drawable::LinesDrawable> _zAxisDrawable;
};

}
} // namespace Lattice
