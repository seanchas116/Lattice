#pragma once

#include "Tool.hpp"
#include "../../state/Tool.hpp"
#include "../../viewport/Renderable.hpp"
#include "../../draw/Vertex.hpp"
#include "../../support/Shorthands.hpp"
#include "../../support/Box.hpp"
#include "../../support/Location.hpp"
#include "../../gl/ContextRecallable.hpp"
#include <glm/glm.hpp>
#include <unordered_map>
#include <QOpenGLExtraFunctions>

namespace Lattice {

namespace State {
class AppState;
class MeshEditState;
}

namespace Document {
class MeshObject;
}

namespace GL {
struct Vertex;
template <typename T> class VertexBuffer;
class IndexBuffer;
class VAO;
class Framebuffer;
}

namespace Editor {

namespace Manipulator {
class MeshManipulator;
}

namespace MeshEditor {

class MeshEditor final : public Viewport::Renderable, protected GL::ContextRecallable, protected QOpenGLExtraFunctions {
    Q_OBJECT
public:
    MeshEditor(const SP<State::AppState>& appState, const SP<State::MeshEditState>& meshEditState);

    auto& meshEditState() const { return _meshEditState; }

    void draw(const SP<Draw::Operations> &operations, const SP<Camera> &camera) override;
    void drawHitArea(const SP<Draw::Operations> &operations, const SP<Camera> &camera) override;
    void drawCustomFramebuffer(const SP<Draw::Operations> &operations, const SP<Camera> &camera) override;

    void mousePressEvent(const Viewport::MouseEvent &event) override;
    void mouseMoveEvent(const Viewport::MouseEvent &event) override;
    void mouseReleaseEvent(const Viewport::MouseEvent &event) override;

    void contextMenuEvent(const Viewport::MouseEvent &event) override;

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    void handleToolChange(State::Tool tool);

    Tool::EventTarget pickEventTarget(glm::vec2 pos);

    void mousePressTarget(const Tool::EventTarget& target, const Viewport::MouseEvent &event);
    void mouseMoveTarget(const Tool::EventTarget& target, const Viewport::MouseEvent &event);
    void mouseReleaseTarget(const Tool::EventTarget& target, const Viewport::MouseEvent &event);

    void hoverEnterTarget(const Tool::EventTarget& target, const Viewport::MouseEvent &event);
    void hoverLeaveTarget(const Tool::EventTarget& target);

    void contextMenuTarget(const Tool::EventTarget& target, const Viewport::MouseEvent& event);

    void updateVAOs();
    void updateManipulatorVisibility();
    void updateChildren();

    void resizeFramebuffers(glm::ivec2 size);

    void handleMeshChanged();

    SP<State::AppState> _appState;
    SP<State::MeshEditState> _meshEditState;

    SP<Manipulator::MeshManipulator> _manipulator;

    bool _isVAOsDirty = true;
    std::unordered_map<Mesh::MaterialHandle, SP<GL::VAO>> _finalShapeVAOs;

    SP<GL::IndexBuffer> _faceIBO;
    SP<GL::VertexBuffer<Draw::Vertex>> _faceVBO;
    SP<GL::VAO> _faceVAO;
    SP<GL::VertexBuffer<Draw::Vertex>> _facePickVBO;
    SP<GL::VAO> _facePickVAO;

    SP<GL::VertexBuffer<Draw::PointLineVertex>> _edgeVBO;
    SP<GL::VAO> _edgeVAO;
    SP<GL::VertexBuffer<Draw::PointLineVertex>> _edgePickVBO;
    SP<GL::VAO> _edgePickVAO;

    SP<GL::VertexBuffer<Draw::PointLineVertex>> _vertexVBO;
    SP<GL::VAO> _vertexVAO;
    SP<GL::VertexBuffer<Draw::PointLineVertex>> _vertexPickVBO;
    SP<GL::VAO> _vertexPickVAO;

    SP<Tool> _tool;

    glm::ivec2 _framebufferSize {0};

    SP<GL::Framebuffer> _facesFramebuffer;
    SP<GL::Texture> _facesTexture;
    SP<GL::Texture> _facesDepthTexture;

    SP<GL::Framebuffer> _vertexHitFramebuffer;
    SP<GL::Framebuffer> _edgeHitFramebuffer;
    SP<GL::Framebuffer> _faceHitFramebuffer;

    Tool::EventTarget _lastMouseMoveTarget;
    Tool::EventTarget _hoveredTarget;
};

}
}
}
