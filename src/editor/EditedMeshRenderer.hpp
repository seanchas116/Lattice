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

namespace OldGL {
class VAO;
class LineVAO;
class PointVAO;
}

namespace Editor {

class MeshPicker;

class EditedMeshRenderer final : public Render::Renderable, public OldGL::ContextRecallable {
    Q_OBJECT
public:
    EditedMeshRenderer(const SP<UI::AppState>& appState, const SP<Document::MeshItem>& item);

    auto& item() const { return _item; }

    void draw(const SP<Render::Operations> &operations, const SP<Camera> &camera) override;

    std::optional<Render::HitResult> hitTest(glm::dvec2 pos, const SP<Camera>& camera) const override;
    void mousePress(const Render::MouseEvent &event) override;
    void mouseMove(const Render::MouseEvent &event) override;
    void mouseRelease(const Render::MouseEvent &event) override;

private:
    void updateWholeVAOs();

    SP<UI::AppState> _appState;
    SP<Document::MeshItem> _item;
    SP<MeshPicker> _meshPicker;
    std::unordered_map<SP<Document::MeshMaterial>, SP<OldGL::VAO>> _faceVAOs;
    SP<OldGL::VertexBuffer> _faceVBO;
    std::vector<OldGL::VertexBuffer::Vertex> _faceAttributes;
    SP<OldGL::LineVAO> _edgeVAO;
    std::vector<OldGL::VertexBuffer::Vertex> _edgeAttributes;
    SP<OldGL::PointVAO> _vertexVAO;
    std::vector<OldGL::VertexBuffer::Vertex> _vertexAttributes;

    std::unordered_map<SP<Document::MeshVertex>, glm::dvec3> _initialPositions;
    glm::dvec3 _dragStartWorldPos;
    double _dragDepth;
    bool _dragStarted;
};

}
}
