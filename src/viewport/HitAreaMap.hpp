#pragma once

#include "../gl/ContextRecallable.hpp"
#include "../support/Shorthands.hpp"
#include "HitResult.hpp"
#include <QOpenGLExtraFunctions>
#include <QVector>
#include <QtGlobal>
#include <glm/glm.hpp>

namespace Lattice {
class Camera;

namespace GL {
class Framebuffer;
}

namespace Draw {
class Operations;
}

namespace Viewport {

class Renderable;
struct DrawEvent;

class HitAreaMap final : protected GL::ContextRecallable {
    Q_DISABLE_COPY(HitAreaMap)
  public:
    HitAreaMap();

    Opt<HitResult> pick(glm::vec2 physicalPos);
    void draw(const SP<Renderable> &renderable, const DrawEvent &drawEvent);

  private:
    void resize(glm::ivec2 size);

    SP<GL::Framebuffer> _framebuffer;
    glm::ivec2 _framebufferSize = {0, 0};
    std::vector<SP<Renderable>> _lastRenderables;
};

} // namespace Viewport
} // namespace Lattice
