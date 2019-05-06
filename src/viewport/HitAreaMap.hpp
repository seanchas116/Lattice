#pragma once

#include "HitResult.hpp"
#include "../support/Shorthands.hpp"
#include "../gl/ContextRecallable.hpp"
#include <QtGlobal>
#include <QVector>
#include <QOpenGLExtraFunctions>
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

class HitAreaMap final : protected QOpenGLExtraFunctions, protected GL::ContextRecallable {
    Q_DISABLE_COPY(HitAreaMap)
public:
    HitAreaMap();

    Opt<HitResult> pick(glm::vec2 physicalPos);
    void draw(const SP<Renderable> &renderable, const SP<Draw::Operations>& operations, const SP<Camera>& camera);

private:
    void resize(glm::ivec2 size);

    SP<GL::Framebuffer> _framebuffer;
    SP<GL::Framebuffer> _additionalInfoFramebuffer;
    glm::ivec2 _framebufferSize = {0, 0};
    std::vector<SP<Renderable>> _lastRenderables;
};

}
}
