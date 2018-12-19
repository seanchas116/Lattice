#pragma once

#include "../support/Camera.hpp"
#include "../support/Pointer.hpp"
#include <QObject>
#include <QOpenGLExtraFunctions>
#include <glm/glm.hpp>
#include <unordered_map>

namespace Lattice {

class ThickLineShader;
class SolidShader;
class CircleShader;
class GridFloor;
class MeshRenderer;

class MeshItem;
class AppState;

class ViewportRenderer final : public QObject, protected QOpenGLExtraFunctions {
    Q_OBJECT
public:
    ViewportRenderer(const SP<AppState>& appState);

    void resize(glm::ivec2 physicalSize, glm::ivec2 logicalSize);
    void render();

    void setCamera(const Camera& camera) { _camera = camera; }

private:
    glm::ivec2 _logicalSize;
    Camera _camera;
    glm::mat4 _projection;
    SP<CircleShader> _circleShader;
    SP<ThickLineShader> _lineShader;
    SP<SolidShader> _solidShader;
    SP<GridFloor> _gridFloor;
    std::unordered_map<SP<MeshItem>, SP<MeshRenderer>> _meshRenderers;
    SP<AppState> _appState;
};

} // namespace Lattice
