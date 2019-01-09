#pragma once

#include "../support/Camera.hpp"
#include "../support/Pointer.hpp"
#include <QObject>
#include <QOpenGLExtraFunctions>
#include <glm/glm.hpp>
#include <unordered_map>

class QMouseEvent;

namespace Lattice::UI {
class AppState;
}

namespace Lattice::Document {
class MeshItem;
}

namespace Lattice::Render {
class Operations;
}

namespace Lattice::Editor {

class GridFloor;
class MeshRenderer;
class Operations;
class OldManipulator;
class OldManipulatorController;
class ItemPicker;
class ItemInteractor;

class ViewportRenderer final : public QObject, protected QOpenGLExtraFunctions {
    Q_OBJECT
public:
    ViewportRenderer(const SP<UI::AppState>& appState);

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
    SP<UI::AppState> _appState;
    SP<Render::Operations> _operations;
    SP<GridFloor> _gridFloor;
    SP<OldManipulator> _manipulator;
    SP<OldManipulatorController> _manipulatorController;
    std::unordered_map<SP<Document::MeshItem>, SP<MeshRenderer>> _meshRenderers;
    SP<ItemPicker> _itemPicker;
    SP<ItemInteractor> _itemInteractor;
    bool _manipulatorDragged = false;
    bool _itemInteractorDragged = false;
};

} // namespace Lattice
