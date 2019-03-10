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

class MeshEditor final : public Render::RenderableObject, protected GL::ContextRecallable, protected QOpenGLExtraFunctions {
    Q_OBJECT
public:
    MeshEditor(const SP<UI::AppState>& appState, const SP<Document::MeshItem>& item);

    auto& item() const { return _item; }

    void draw(const SP<Render::Operations> &operations, const SP<Camera> &camera) override;
    void drawPickables(const SP<Render::Operations> &operations, const SP<Camera> &camera) override;

    void mousePress(const Render::MouseEvent &event) override;
    void mouseMove(const Render::MouseEvent &event) override;
    void mouseRelease(const Render::MouseEvent &event) override;
    void hoverMove(const Render::MouseEvent &event) override;

private:
    void updateWholeVAOs();

    struct EventTarget {
        Opt<SP<Mesh::Vertex>> vertex;
        Opt<SP<Mesh::Edge>> edge;
        Opt<SP<Mesh::Face>> face;
    };

    void mousePressTarget(const EventTarget& target, const Render::MouseEvent &event);
    void mouseMoveTarget(const EventTarget& target, const Render::MouseEvent &event);
    void mouseReleaseTarget(const EventTarget& target, const Render::MouseEvent &event);
    void hoverEnterTarget(const EventTarget& target, const Render::MouseEvent &event);
    void hoverMoveTarget(const EventTarget& target, const Render::MouseEvent &event);
    void hoverLeaveTarget(const EventTarget& target);

    void vertexDragStart(const std::unordered_set<SP<Mesh::Vertex>>& vertices, const Render::MouseEvent& event);
    void vertexDragMove(const Render::MouseEvent& event);

    class VertexPickable;
    class EdgePickable;
    class FacePickable;

    SP<UI::AppState> _appState;
    SP<Document::MeshItem> _item;

    std::unordered_map<SP<Mesh::Material>, SP<GL::VAO>> _faceVAOs;
    SP<GL::VertexBuffer<GL::Vertex>> _faceVBO;
    std::vector<GL::Vertex> _faceAttributes;
    SP<GL::VAO> _facePickVAO;
    std::vector<GL::Vertex> _facePickAttributes;

    SP<GL::VAO> _edgeVAO;
    std::vector<GL::Vertex> _edgeAttributes;
    SP<GL::VAO> _edgePickVAO;
    std::vector<GL::Vertex> _edgePickAttributes;

    SP<GL::VAO> _vertexVAO;
    std::vector<GL::Vertex> _vertexAttributes;
    SP<GL::VAO> _vertexPickVAO;
    std::vector<GL::Vertex> _vertexPickAttributes;

    // vertex drag
    std::unordered_map<SP<Mesh::Vertex>, glm::dvec3> _dragInitPositions;
    glm::dvec3 _dragInitWorldPos;
    bool _dragStarted;

    // vertex hover
    Opt<SP<Mesh::Vertex>> _hoveredVertex;
    Opt<SP<Mesh::Edge>> _hoveredEdge;
    // TODO: hovered face

    // draw
    std::vector<SP<Mesh::UVPoint>> _drawnUVPoints;
    Opt<SP<Mesh::UVPoint>> lastDrawnPoint() const;
    Opt<SP<Mesh::Vertex>> lastDrawnVertex() const;
};

}
}
