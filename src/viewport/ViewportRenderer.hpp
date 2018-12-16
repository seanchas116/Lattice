#pragma once

#include <QObject>
#include <QOpenGLExtraFunctions>
#include <glm/glm.hpp>
#include "../support/Pointer.hpp"

namespace Lattice {

class Shader;

class ViewportRenderer final : public QObject, protected QOpenGLExtraFunctions {
    Q_OBJECT
public:
    ViewportRenderer();

    void resize(glm::ivec2 size);
    void render();

private:
    SP<Shader> _lineShader;
};

} // namespace Lattice
