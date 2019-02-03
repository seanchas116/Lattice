#pragma once

#include "../support/Pointer.hpp"
#include "../support/Box.hpp"
#include "../support/Location.hpp"
#include "../render/Renderable.hpp"
#include "../oldgl/ContextRecallable.hpp"
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

namespace OldGL {
class VAO;
class LineVAO;
class PointVAO;
}

namespace Editor {

class MeshPicker;

class MeshRenderer final : public Render::Renderable, public OldGL::ContextRecallable {
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
    std::unordered_map<SP<Document::MeshMaterial>, SP<OldGL::VAO>> _faceVAOs;
    SP<OldGL::LineVAO> _edgeVAO;
    SP<OldGL::PointVAO> _vertexVAO;

    Location _dragInitLocation;
    glm::dvec3 _dragInitWorldPos;
    double _dragInitDepth;
    bool _dragStarted;
};

}
}
