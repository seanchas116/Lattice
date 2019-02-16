#pragma once

#include "../support/Shorthands.hpp"
#include "../support/Box.hpp"
#include "../support/Location.hpp"
#include "../render/RenderableObject.hpp"
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
}

namespace Editor {

class MeshPicker;

class MeshEditor final : public Render::RenderableObject, public GL::ContextRecallable {
    Q_OBJECT
public:
    MeshEditor(const SP<UI::AppState>& appState, const SP<Document::MeshItem>& item);

    auto& item() const { return _item; }

    void draw(const SP<Render::Operations> &operations, const SP<Camera> &camera) override;

    Opt<Render::HitResult> hitTest(glm::dvec2 pos, const SP<Camera>& camera) const override;
    void mousePress(const Render::MouseEvent &event) override;
    void mouseMove(const Render::MouseEvent &event) override;
    void mouseRelease(const Render::MouseEvent &event) override;
    void hoverEnter(const Render::MouseEvent &event) override;
    void hoverMove(const Render::MouseEvent &event) override;
    void hoverLeave() override;

private:
    void updateWholeVAOs();

    void vertexDragStart(const std::unordered_set<SP<Document::MeshVertex>>& vertices, const Render::MouseEvent& event);
    void vertexDragMove(const Render::MouseEvent& event);

    class VertexPickable;
    class EdgePickable;
    class FacePickable;

    SP<UI::AppState> _appState;
    SP<Document::MeshItem> _item;
    SP<MeshPicker> _meshPicker;
    std::unordered_map<SP<Document::MeshMaterial>, SP<GL::VAO>> _faceVAOs;
    SP<GL::VertexBuffer<GL::Vertex>> _faceVBO;
    std::vector<GL::Vertex> _faceAttributes;
    SP<GL::VAO> _edgeVAO;
    std::vector<GL::Vertex> _edgeAttributes;
    SP<GL::VAO> _vertexVAO;
    std::vector<GL::Vertex> _vertexAttributes;

    std::unordered_map<SP<Document::MeshVertex>, glm::dvec3> _dragInitPositions;
    glm::dvec3 _dragInitWorldPos;
    bool _dragStarted;

    Opt<SP<Document::MeshVertex>> _hoveredVertex;
    Opt<SP<Document::MeshEdge>> _hoveredEdge;
    // TODO: hovered face
};

}
}
