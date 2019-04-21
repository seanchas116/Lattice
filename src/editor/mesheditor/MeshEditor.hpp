#pragma once

#include "Tool.hpp"
#include "../../viewport/RenderableObject.hpp"
#include "../../gl/Vertex.hpp"
#include "../../support/Shorthands.hpp"
#include "../../support/Box.hpp"
#include "../../support/Location.hpp"
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
struct Vertex;
template <typename T> class VertexBuffer;
class IndexBuffer;
class VAO;
}

namespace Editor {
namespace Manipulator {
class MeshManipulator;
}

namespace MeshEditor {

class MeshEditor final : public Viewport::RenderableObject, protected QOpenGLExtraFunctions {
    Q_OBJECT
public:
    MeshEditor(const SP<State::AppState>& appState, const SP<Document::MeshObject>& object);

    auto& object() const { return _object; }

    void draw(const SP<Draw::Operations> &operations, const SP<Camera> &camera) override;
    void drawPickables(const SP<Draw::Operations> &operations, const SP<Camera> &camera) override;

    void mousePressEvent(const Viewport::MouseEvent &event) override;
    void mouseMoveEvent(const Viewport::MouseEvent &event) override;
    void mouseReleaseEvent(const Viewport::MouseEvent &event) override;

    void contextMenuEvent(const Viewport::ContextMenuEvent &event) override;

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    void handleToolChange(State::Tool tool);
    void handleMeshChange();

    void mousePressTarget(const Tool::EventTarget& target, const Viewport::MouseEvent &event);
    void mouseMoveTarget(const Tool::EventTarget& target, const Viewport::MouseEvent &event);
    void mouseReleaseTarget(const Tool::EventTarget& target, const Viewport::MouseEvent &event);

    void hoverEnterTarget(const Tool::EventTarget& target, const Viewport::MouseEvent &event);
    void hoverLeaveTarget(const Tool::EventTarget& target);

    void contextMenuTarget(const Tool::EventTarget& target, const Viewport::ContextMenuEvent& event);

    void updateVAOTopology();
    void updateVAOAttributes();

    void updateWholeVAOs();
    void updateManinpulatorVisibility();
    void updateChildren();

    class EditorPickable;
    class VertexPickable;
    class EdgePickable;
    class FacePickable;

    SP<State::AppState> _appState;
    SP<Document::MeshObject> _object;

    SP<Manipulator::MeshManipulator> _manipulator;

    bool _isVAOsDirty = true;

    std::vector<SP<Mesh::Face>> _faces;
    std::vector<SP<FacePickable>> _facePickables;

    SP<GL::IndexBuffer> _faceIndexBuffer;

    std::vector<GL::Vertex> _faceAttributes;
    SP<GL::VertexBuffer<GL::Vertex>> _faceVertexBuffer;
    SP<GL::VAO> _faceVAO;

    std::vector<GL::Vertex> _facePickAttributes;
    SP<GL::VertexBuffer<GL::Vertex>> _facePickVertexBuffer;
    SP<GL::VAO> _facePickVAO;

    std::vector<SP<Mesh::Edge>> _edges;
    std::vector<SP<EdgePickable>> _edgePickables;

    std::vector<GL::Vertex> _edgeAttributes;
    SP<GL::VertexBuffer<GL::Vertex>> _edgeVertexBuffer;
    SP<GL::VAO> _edgeVAO;

    std::vector<GL::Vertex> _edgePickAttributes;
    SP<GL::VertexBuffer<GL::Vertex>> _edgePickVertexBuffer;
    SP<GL::VAO> _edgePickVAO;

    std::vector<SP<Mesh::Vertex>> _vertices;
    std::vector<SP<VertexPickable>> _vertexPickables;

    std::vector<GL::Vertex> _vertexAttributes;
    SP<GL::VertexBuffer<GL::Vertex>> _vertexVertexBuffer;
    SP<GL::VAO> _vertexVAO;

    std::vector<GL::Vertex> _vertexPickAttributes;
    SP<GL::VertexBuffer<GL::Vertex>> _vertexPickVertexBuffer;
    SP<GL::VAO> _vertexPickVAO;

    std::vector<SP<Viewport::Renderable>> _pickables;

    SP<Tool> _tool;

    // vertex hover
    Opt<SP<Mesh::Vertex>> _hoveredVertex;
    Opt<SP<Mesh::Edge>> _hoveredEdge;
    Opt<SP<Mesh::Face>> _hoveredFace;
};

}
}
}
