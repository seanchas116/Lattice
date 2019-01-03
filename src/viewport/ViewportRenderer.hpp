#pragma once

#include "../support/Camera.hpp"
#include "../support/Pointer.hpp"
#include <QObject>
#include <QOpenGLExtraFunctions>
#include <glm/glm.hpp>
#include <unordered_map>

class QMouseEvent;

namespace Lattice {
class AppState;
}

namespace Lattice::Document {
class MeshItem;
}

namespace Lattice::Viewport {

class GridFloor;
class MeshRenderer;
class Operations;
class Manipulator;
class ManipulatorController;

class ViewportRenderer final : public QObject, protected QOpenGLExtraFunctions {
    Q_OBJECT
public:
    ViewportRenderer(const SP<AppState>& appState);

    void resize(glm::ivec2 physicalSize, glm::ivec2 logicalSize);
    void render();

    void setCameraLocation(const Location& location) { _camera.setLocation(location); }

    void mousePress(QMouseEvent* event, glm::dvec2 pos);
    void mouseMove(QMouseEvent* event, glm::dvec2 pos);
    void mouseRelease(QMouseEvent* event, glm::dvec2 pos);

signals:
    void updateNeeded();

private:
    glm::ivec2 _logicalSize;
    Camera _camera;
    SP<Operations> _operations;
    SP<GridFloor> _gridFloor;
    SP<Manipulator> _manipulator;
    SP<ManipulatorController> _manipulatorController;
    std::unordered_map<SP<Document::MeshItem>, SP<MeshRenderer>> _meshRenderers;
    SP<AppState> _appState;
};

} // namespace Lattice
