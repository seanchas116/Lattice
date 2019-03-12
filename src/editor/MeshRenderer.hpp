#pragma once

#include "../support/Shorthands.hpp"
#include "../support/Box.hpp"
#include "../support/Location.hpp"
#include "../render/RenderableObject.hpp"
#include "../gl/ContextRecallable.hpp"
#include <glm/glm.hpp>
#include <unordered_map>

class QMouseEvent;

namespace Lattice {

class Camera;

namespace UI {
class AppState;
}

namespace Document {
class MeshItem;
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

class MeshRenderer final : public Render::RenderableObject, public GL::ContextRecallable {
    Q_OBJECT
public:
    MeshRenderer(const SP<UI::AppState>& appState, const SP<Document::MeshItem>& item);

    void draw(const SP<Render::Operations> &operations, const SP<Camera> &camera) override;
    void drawPickables(const SP<Render::Operations> &operations, const SP<Camera> &camera) override;

    void mousePress(const Render::MouseEvent &event) override;
    void mouseMove(const Render::MouseEvent &event) override;
    void mouseRelease(const Render::MouseEvent &event) override;
    void mouseDoubleClick(const Render::MouseEvent &event) override;

private:
    void updateVAOs();

    SP<UI::AppState> _appState;
    SP<Document::MeshItem> _item;
    std::unordered_map<SP<Mesh::Material>, SP<GL::VAO>> _faceVAOs;
    SP<GL::VAO> _edgeVAO;
    SP<GL::VAO> _vertexVAO;

    bool _dragged = false;
    Location _dragInitLocation;
    glm::dvec3 _dragInitWorldPos;
    bool _dragStarted = false;
};

}
}
