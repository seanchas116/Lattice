#pragma once

#include "Tool.hpp"
#include "../../support/Shorthands.hpp"
#include "../../support/Box.hpp"
#include "../../support/Location.hpp"
#include "../../render/RenderableObject.hpp"
#include "../../gl/ContextRecallable.hpp"
#include "../../gl/VertexBuffer.hpp"
#include <glm/glm.hpp>
#include <unordered_map>

class QMouseEvent;

namespace Lattice {

class Camera;

namespace State {
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
namespace MeshEditor {

class MoveTool;
class DrawTool;

class MeshEditor final : public Render::RenderableObject, protected GL::ContextRecallable, protected QOpenGLExtraFunctions {
    Q_OBJECT
public:
    MeshEditor(const SP<State::AppState>& appState, const SP<Document::MeshItem>& item);

    auto& item() const { return _item; }

    void draw(const SP<Render::Operations> &operations, const SP<Camera> &camera) override;
    void drawPickables(const SP<Render::Operations> &operations, const SP<Camera> &camera) override;

    void mousePress(const Render::MouseEvent &event) override;
    void mouseMove(const Render::MouseEvent &event) override;
    void mouseRelease(const Render::MouseEvent &event) override;

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    void handleToolChange(State::Tool tool);
    void handleMeshChange();

    void mousePressTarget(const Tool::EventTarget& target, const Render::MouseEvent &event);
    void mouseMoveTarget(const Tool::EventTarget& target, const Render::MouseEvent &event);
    void mouseReleaseTarget(const Tool::EventTarget& target, const Render::MouseEvent &event);
    void hoverEnterTarget(const Tool::EventTarget& target, const Render::MouseEvent &event);
    void hoverLeaveTarget(const Tool::EventTarget& target);

    void updateWholeVAOs();

    class VertexPickable;
    class EdgePickable;
    class FacePickable;

    SP<State::AppState> _appState;
    SP<Document::MeshItem> _item;

    bool _isMeshDirty = true;

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

    SP<Tool> _tool;

    // vertex hover
    Opt<SP<Mesh::Vertex>> _hoveredVertex;
    Opt<SP<Mesh::Edge>> _hoveredEdge;
    // TODO: hovered face
};

}
}
}
