#pragma once

#include "../support/Camera.hpp"
#include "../support/Pointer.hpp"
#include <QObject>
#include <QOpenGLExtraFunctions>
#include <glm/glm.hpp>
#include <unordered_map>

class QMouseEvent;

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

    void setCameraLocation(const Location& location) { _camera.setLocation(location); }

    void mousePress(QMouseEvent* event, glm::vec2 pos);
    void mouseMove(QMouseEvent* event, glm::vec2 pos);
    void mouseRelease(QMouseEvent* event, glm::vec2 pos);

signals:
    void updateNeeded();

private:
    glm::ivec2 _logicalSize;
    Camera _camera;
    SP<Operations> _operations;
    SP<GridFloor> _gridFloor;
    SP<Manipulator> _manipulator;
    std::unordered_map<SP<MeshItem>, SP<MeshRenderer>> _meshRenderers;
    SP<AppState> _appState;
};

} // namespace Lattice
