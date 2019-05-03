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
    void handleMeshTopologyChange();
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
    SP<Mesh::Mesh> _mesh;

    SP<Manipulator::MeshManipulator> _manipulator;

    bool _isVAODirty = true;
    SP<GL::VAO> _facePickVAO;
    SP<GL::VAO> _edgeVAO;
    SP<GL::VAO> _edgePickVAO;
    SP<GL::VAO> _vertexVAO;
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
