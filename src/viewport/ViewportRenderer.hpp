#pragma once

#include "../support/OldCamera.hpp"
#include "../support/Camera.hpp"
#include "../support/Pointer.hpp"
#include <QObject>
#include <QOpenGLExtraFunctions>
#include <glm/glm.hpp>
#include <unordered_map>

namespace Lattice {

class GridFloor;
class MeshRenderer;
class Operations;
class Manipulator;

class MeshItem;
class AppState;

class ViewportRenderer final : public QObject, protected QOpenGLExtraFunctions {
    Q_OBJECT
public:
    ViewportRenderer(const SP<AppState>& appState);

    void resize(glm::ivec2 physicalSize, glm::ivec2 logicalSize);
    void render();

    void setCamera(const OldCamera& camera) { _camera = camera; }

signals:
    void updateNeeded();

private:
    glm::ivec2 _logicalSize;
    OldCamera _camera;
    Camera _projection;
    SP<Operations> _operations;
    SP<GridFloor> _gridFloor;
    SP<Manipulator> _manipulator;
    std::unordered_map<SP<MeshItem>, SP<MeshRenderer>> _meshRenderers;
    SP<AppState> _appState;
};

} // namespace Lattice
