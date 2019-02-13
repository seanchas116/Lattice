#pragma once

#include "../support/Shorthands.hpp"
#include <QtGlobal>
#include <QVector>
#include <QOpenGLExtraFunctions>
#include <glm/glm.hpp>

namespace Lattice {
class Camera;
}

namespace Lattice::GL {
class Framebuffer;
}

namespace Lattice::Render {

class Pickable;
class Renderable;
class Operations;

class PickableMap final : protected QOpenGLExtraFunctions {
    Q_DISABLE_COPY(PickableMap)
public:
    PickableMap();

    void resize(glm::ivec2 size);
    SP<Pickable> pick(glm::vec2 physicalPos);
    void draw(const std::vector<SP<Renderable> > &renderables, const SP<Operations>& operations, const SP<Camera>& camera);

private:
    SP<GL::Framebuffer> _framebuffer;
    glm::ivec2 _framebufferSize = {0, 0};
    std::vector<SP<Pickable>> _pickables;
};

}
