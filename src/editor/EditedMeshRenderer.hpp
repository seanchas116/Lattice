#pragma once

#include "../support/Pointer.hpp"
#include "../support/Box.hpp"
#include "../support/Location.hpp"
#include "../render/Renderable.hpp"
#include "../gl/ContextRecallable.hpp"
#include "../gl/VertexBuffer.hpp"
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
class LineVAO;
class PointVAO;
}

namespace Editor {

class MeshPicker;

class EditedMeshRenderer final : public Render::Renderable, public GL::ContextRecallable {
    Q_OBJECT
public:
    EditedMeshRenderer(const SP<UI::AppState>& appState, const SP<Document::MeshItem>& item);

    auto& item() const { return _item; }

    void draw(const SP<Render::Operations> &operations, const Camera &camera) override;

    std::optional<Render::HitResult> hitTest(glm::dvec2 pos, const Camera& camera) const override;
    void mousePress(const Render::MouseEvent &event) override;
    void mouseMove(const Render::MouseEvent &event) override;
    void mouseRelease(const Render::MouseEvent &event) override;

private:
    void updateVAOs();

    SP<UI::AppState> _appState;
    SP<Document::MeshItem> _item;
    SP<MeshPicker> _meshPicker;
    std::unordered_map<SP<Document::MeshMaterial>, SP<GL::VAO>> _faceVAOs;
    SP<GL::LineVAO> _edgeVAO;
    SP<GL::PointVAO> _vertexVAO;
    std::vector<GL::VertexBuffer::Vertex> _vertexAttributes;

    std::unordered_map<SP<Document::MeshVertex>, glm::dvec3> _initialPositions;
    glm::dvec3 _dragStartWorldPos;
    double _dragDepth;
    bool _dragStarted;
};

}
}
