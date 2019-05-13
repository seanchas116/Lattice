#pragma once

#include "../support/Shorthands.hpp"
#include "../support/Box.hpp"
#include "../support/Location.hpp"
#include "../viewport/Renderable.hpp"
#include <glm/glm.hpp>
#include <unordered_map>

class QMouseEvent;

namespace Lattice {

class Camera;

namespace State {
class AppState;
}

namespace Document {
class MeshObject;
}

namespace GL {
class VAO;
}

namespace Editor {

class MeshPicker;

class MeshRenderer final : public Viewport::Renderable {
    Q_OBJECT
public:
    MeshRenderer(const SP<State::AppState>& appState, const SP<Document::MeshObject>& object);

    void draw(const SP<Draw::Operations> &operations, const SP<Camera> &camera) override;
    void drawHitArea(const SP<Draw::Operations> &operations, const SP<Camera> &camera) override;

    void mousePressEvent(const Viewport::MouseEvent &event) override;
    void mouseMoveEvent(const Viewport::MouseEvent &event) override;
    void mouseReleaseEvent(const Viewport::MouseEvent &event) override;
    void mouseDoubleClickEvent(const Viewport::MouseEvent &event) override;

private:
    void updateVAOs();

    SP<State::AppState> _appState;
    SP<Document::MeshObject> _object;

    std::unordered_map<uint32_t, SP<GL::VAO>> _faceVAOs;
    bool _isVAOsDirty = true;

    bool _dragged = false;
    Location _dragInitLocation;
    glm::dvec3 _dragInitWorldPos;
    glm::dvec2 _dragInitViewportPos;
    bool _dragStarted = false;
};

}
}
