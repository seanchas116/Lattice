#pragma once

#include <QObject>
#include <QOpenGLExtraFunctions>
#include <glm/glm.hpp>
#include "../support/Camera.hpp"
#include "../support/Pointer.hpp"

namespace Lattice {

class Shader;
class GridFloor;

class ViewportRenderer final : public QObject, protected QOpenGLExtraFunctions {
    Q_OBJECT
public:
    ViewportRenderer();

    void resize(glm::ivec2 size);
    void render();

private:
    glm::ivec2 _size;
    Camera _camera;
    glm::mat4 _projection;
    SP<Shader> _lineShader;
    SP<Shader> _solidShader;
    SP<GridFloor> _gridFloor;
};

} // namespace Lattice
