#pragma once

#include "Tool.hpp"
#include "../../viewport/RenderableObject.hpp"
#include "../../draw/Vertex.hpp"
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

    void mousePressTarget(const Tool::EventTarget& target, const Viewport::MouseEvent &event);
    void mouseMoveTarget(const Tool::EventTarget& target, const Viewport::MouseEvent &event);
    void mouseReleaseTarget(const Tool::EventTarget& target, const Viewport::MouseEvent &event);

    void hoverEnterTarget(const Tool::EventTarget& target, const Viewport::MouseEvent &event);
    void hoverLeaveTarget(const Tool::EventTarget& target);

    void contextMenuTarget(const Tool::EventTarget& target, const Viewport::ContextMenuEvent& event);

    void updateVAOs();
    void updateManinpulatorVisibility();
    void updateChildren();

    class EditorPickable;
    class VertexPickable;
    class EdgePickable;
    class FacePickable;

    SP<State::AppState> _appState;
    SP<Document::MeshObject> _object;
    SP<Mesh::Mesh> _mesh;

    SP<Manipulator::MeshManipulator> _manipulator;

    bool _isVAOsDirty = true;
    SP<GL::VertexBuffer<Draw::Vertex>> _facePickVBO;
    SP<GL::IndexBuffer> _faceIBO;
    SP<GL::VAO> _facePickVAO;
    SP<GL::VertexBuffer<Draw::PointLineVertex>> _edgeVBO;
    SP<GL::VAO> _edgeVAO;
    SP<GL::VertexBuffer<Draw::PointLineVertex>> _edgePickVBO;
    SP<GL::VAO> _edgePickVAO;
    SP<GL::VertexBuffer<Draw::PointLineVertex>> _vertexVBO;
    SP<GL::VAO> _vertexVAO;
    SP<GL::VertexBuffer<Draw::PointLineVertex>> _vertexPickVBO;
    SP<GL::VAO> _vertexPickVAO;

    std::vector<SP<Viewport::Renderable>> _pickables;

    SP<Tool> _tool;

    // vertex hover
    Opt<Mesh::VertexHandle> _hoveredVertex;
    Opt<Mesh::EdgeHandle> _hoveredEdge;
    Opt<Mesh::FaceHandle> _hoveredFace;
};

}
}
}
