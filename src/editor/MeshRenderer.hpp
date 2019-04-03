#pragma once

#include "../support/Shorthands.hpp"
#include "../support/Box.hpp"
#include "../support/Location.hpp"
#include "../render/RenderableObject.hpp"
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

namespace Mesh {
class Mesh;
class Material;
}

namespace GL {
class VAO;
}

namespace Editor {

class MeshPicker;

class MeshRenderer final : public Render::RenderableObject {
    Q_OBJECT
public:
    MeshRenderer(const SP<State::AppState>& appState, const SP<Document::MeshObject>& item);

    void draw(const SP<Render::Operations> &operations, const SP<Camera> &camera) override;
    void drawPickables(const SP<Render::Operations> &operations, const SP<Camera> &camera) override;

    void mousePressEvent(const Render::MouseEvent &event) override;
    void mouseMoveEvent(const Render::MouseEvent &event) override;
    void mouseReleaseEvent(const Render::MouseEvent &event) override;
    void mouseDoubleClickEvent(const Render::MouseEvent &event) override;

private:
    void updateVAOs();

    SP<State::AppState> _appState;
    SP<Document::MeshObject> _item;

    std::unordered_map<SP<Mesh::Material>, SP<GL::VAO>> _faceVAOs;
    SP<GL::VAO> _edgeVAO;
    SP<GL::VAO> _vertexVAO;
    bool _isVAOsDirty = true;

    bool _dragged = false;
    Location _dragInitLocation;
    glm::dvec3 _dragInitWorldPos;
    glm::dvec2 _dragInitViewportPos;
    bool _dragStarted = false;
};

}
}
