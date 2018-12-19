#pragma once

#include "../support/Camera.hpp"
#include "../support/Pointer.hpp"
#include <QObject>
#include <QOpenGLExtraFunctions>
#include <glm/glm.hpp>
#include <unordered_map>

namespace Lattice {

class Shader;
class ThickLineShader;
class GridFloor;
class MeshRenderer;

class MeshItem;
class AppState;

class ViewportRenderer final : public QObject, protected QOpenGLExtraFunctions {
    Q_OBJECT
public:
    ViewportRenderer(const SP<AppState>& appState);

    void resize(glm::ivec2 size);
    void render();

    void setCamera(const Camera& camera) { _camera = camera; }

private:
    glm::ivec2 _size;
    Camera _camera;
    glm::mat4 _projection;
    SP<ThickLineShader> _lineShader;
    SP<Shader> _solidShader;
    SP<GridFloor> _gridFloor;
    std::unordered_map<SP<MeshItem>, SP<MeshRenderer>> _meshRenderers;
    SP<AppState> _appState;
};

} // namespace Lattice
