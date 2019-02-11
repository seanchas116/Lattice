#pragma once

#include "../support/Pointer.hpp"
#include "../support/Box.hpp"
#include "../support/Location.hpp"
#include "../render/Renderable.hpp"
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
class Mesh;
class MeshMaterial;
}

namespace GL {
class VAO;
}

namespace Editor {

class MeshPicker;

class MeshRenderer final : public Render::Renderable, public GL::ContextRecallable {
    Q_OBJECT
public:
    MeshRenderer(const SP<UI::AppState>& appState, const SP<Document::MeshItem>& item);

    void draw(const SP<Render::Operations> &operations, const SP<Camera> &camera) override;

    std::optional<Render::HitResult> hitTest(glm::dvec2 pos, const SP<Camera>& camera) const override;
    void mousePress(const Render::MouseEvent &event) override;
    void mouseMove(const Render::MouseEvent &event) override;
    void mouseRelease(const Render::MouseEvent &event) override;
    void mouseDoubleClick(const Render::MouseEvent &event) override;

private:
    void updateVAOs();

    SP<UI::AppState> _appState;
    SP<Document::MeshItem> _item;
    SP<MeshPicker> _meshPicker;
    std::unordered_map<SP<Document::MeshMaterial>, SP<GL::VAO>> _faceVAOs;
    SP<GL::VAO> _edgeVAO;
    SP<GL::VAO> _vertexVAO;

    Location _dragInitLocation;
    glm::dvec3 _dragInitWorldPos;
    bool _dragStarted;
};

}
}
