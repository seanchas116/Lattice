#include "MeshEditor.hpp"
#include "MoveTool.hpp"
#include "DrawTool.hpp"
#include "ExtrudeTool.hpp"
#include "LoopCutTool.hpp"
#include "BorderSelectTool.hpp"
#include "../manipulator/MeshManipulator.hpp"
#include "../../state/AppState.hpp"
#include "../../gl/VAO.hpp"
#include "../../gl/VertexBuffer.hpp"
#include "../../document/Document.hpp"
#include "../../document/History.hpp"
#include "../../document/MeshObject.hpp"
#include "../../mesh/Mesh.hpp"
#include "../../oldmesh/Mesh.hpp"
#include "../../support/Debug.hpp"
#include "../../support/Camera.hpp"
#include <QMouseEvent>
#include <QMenu>
#include <QPainter>

using namespace glm;

namespace Lattice::Editor::MeshEditor {

namespace {

const vec4 unselectedColor = vec4(0, 0, 0, 1);
const vec4 selectedColor = vec4(1, 1, 1, 1);
const vec4 hoveredColor = vec4(1, 1, 0, 1);

const vec4 unselectedFaceHighlight = vec4(0, 0, 0, 0);
const vec4 selectedFaceHighlight = vec4(1, 1, 1, 0.5);
const vec4 hoveredFaceHighlight = vec4(1, 1, 0.5, 0.5);

}

class MeshEditor::EditorPickable : public Viewport::Renderable {
public:
    EditorPickable(MeshEditor* editor) : _editor(editor) {}
    void mousePressEvent(const Viewport::MouseEvent &event) override {
        _editor->mousePressTarget(target(), event);
    }
    void mouseMoveEvent(const Viewport::MouseEvent &event) override {
        _editor->mouseMoveTarget(target(), event);
    }
    void mouseReleaseEvent(const Viewport::MouseEvent &event) override {
        _editor->mouseReleaseTarget(target(), event);
    }
    void contextMenuEvent(const Viewport::ContextMenuEvent &event) override {
        _editor->contextMenuTarget(target(), event);
    }
    void hoverEnterEvent(const Viewport::MouseEvent &event) override {
        _editor->hoverEnterTarget(target(), event);
    }
    void hoverLeaveEvent() override {
        _editor->hoverLeaveTarget(target());
    }
protected:
    virtual Tool::EventTarget target() const = 0;
private:
    MeshEditor* _editor;
};

class MeshEditor::VertexPickable : public MeshEditor::EditorPickable {
public:
    VertexPickable(MeshEditor* editor, Mesh::VertexHandle vertex) : EditorPickable(editor), _vertex(vertex) {}
    Tool::EventTarget target() const override {
        return {_vertex, {}, {}};
    }
private:
    Mesh::VertexHandle _vertex;
};

class MeshEditor::EdgePickable : public MeshEditor::EditorPickable {
public:
    EdgePickable(MeshEditor* editor, Mesh::EdgeHandle edge) : EditorPickable(editor), _edge(edge) {}
    Tool::EventTarget target() const override {
        return {{}, _edge, {}};
    }
private:
    Mesh::EdgeHandle _edge;
};

class MeshEditor::FacePickable : public MeshEditor::EditorPickable {
public:
    FacePickable(MeshEditor* editor, Mesh::FaceHandle face) : EditorPickable(editor), _face(face) {}
    Tool::EventTarget target() const override {
        return {{}, {}, _face};
    }
private:
    Mesh::FaceHandle _face;
};

MeshEditor::MeshEditor(const SP<State::AppState>& appState, const SP<Document::MeshObject> &object) :
    _appState(appState),
    _object(object),
    _mesh(makeShared<Mesh::Mesh>(object->mesh())),
    _manipulator(makeShared<Manipulator::MeshManipulator>(appState, object)),

    _facePickVBO(makeShared<GL::VertexBuffer<Draw::Vertex>>()),
    _faceIBO(makeShared<GL::IndexBuffer>()),
    _facePickVAO(makeShared<GL::VAO>(_facePickVBO, _faceIBO)),
    _edgeVBO(makeShared<GL::VertexBuffer<Draw::PointLineVertex>>()),
    _edgeVAO(makeShared<GL::VAO>(_edgeVBO, GL::Primitive::Line)),
    _edgePickVBO(makeShared<GL::VertexBuffer<Draw::PointLineVertex>>()),
    _edgePickVAO(makeShared<GL::VAO>(_edgePickVBO, GL::Primitive::Line)),
    _vertexVBO(makeShared<GL::VertexBuffer<Draw::PointLineVertex>>()),
    _vertexVAO(makeShared<GL::VAO>(_vertexVBO, GL::Primitive::Point)),
    _vertexPickVBO(makeShared<GL::VertexBuffer<Draw::PointLineVertex>>()),
    _vertexPickVAO(makeShared<GL::VAO>(_vertexPickVBO, GL::Primitive::Point)),

    _tool(makeShared<MoveTool>(appState, object, _mesh))
{
    initializeOpenGLFunctions();
    updateVAOs();

    connect(appState.get(), &State::AppState::toolChanged, this, &MeshEditor::handleToolChange);
    handleToolChange(appState->tool());

    connect(appState->document().get(), &Document::Document::meshSelectionChanged, this, &MeshEditor::updateManinpulatorVisibility);
    connect(appState.get(), &State::AppState::toolChanged, this, &MeshEditor::updateManinpulatorVisibility);
    updateManinpulatorVisibility();

    connect(_manipulator.get(), &Manipulator::Manipulator::onContextMenu, this, [this](auto& event) {
        contextMenuTarget({}, event);
    });

    connect(appState.get(), &State::AppState::isTranslateHandleVisibleChanged, _manipulator.get(), &Manipulator::Manipulator::setTranslateHandleVisible);
    connect(appState.get(), &State::AppState::isRotateHandleVisibleChanged, _manipulator.get(), &Manipulator::Manipulator::setRotateHandleVisible);
    connect(appState.get(), &State::AppState::isScaleHandleVisibleChanged, _manipulator.get(), &Manipulator::Manipulator::setScaleHandleVisible);
}

void MeshEditor::draw(const SP<Draw::Operations> &operations, const SP<Camera> &camera) {
    updateVAOs();

    // TODO: Render faces

    operations->drawLine.draw(_edgeVAO, _object->location().matrixToWorld(), camera, 1.0, vec4(0), true);
    if (_appState->isVertexSelectable()) {
        operations->drawCircle.draw(_vertexVAO, _object->location().matrixToWorld(), camera, 6.0, vec4(0), true);
    }
}

void MeshEditor::drawPickables(const SP<Draw::Operations> &operations, const SP<Camera> &camera) {
    updateVAOs();

    auto idColor = toIDColor();
    glClearColor(idColor.r, idColor.g, idColor.b, idColor.a);
    glClearDepthf(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (_appState->isFaceSelectable()) {
        operations->drawUnicolor.draw(_facePickVAO, _object->location().matrixToWorld(), camera, vec4(0), true);
    }
    if (_appState->isEdgeSelectable()) {
        operations->drawLine.draw(_edgePickVAO, _object->location().matrixToWorld(), camera, 12.0, vec4(0), true);
    }
    if (_appState->isVertexSelectable()) {
        operations->drawCircle.draw(_vertexPickVAO, _object->location().matrixToWorld(), camera, 24.0, vec4(0), true);
    }
}

void MeshEditor::mousePressEvent(const Viewport::MouseEvent &event) {
    mousePressTarget({}, event);
}

void MeshEditor::mouseMoveEvent(const Viewport::MouseEvent &event) {
    mouseMoveTarget({}, event);
}

void MeshEditor::mouseReleaseEvent(const Viewport::MouseEvent &event) {
    mouseReleaseTarget({}, event);
}

void MeshEditor::contextMenuEvent(const Viewport::ContextMenuEvent &event) {
    contextMenuTarget({}, event);
}

void MeshEditor::keyPressEvent(QKeyEvent *event) {
    _tool->keyPressTool(event);
}

void MeshEditor::keyReleaseEvent(QKeyEvent *event) {
    _tool->keyReleaseTool(event);
}

void MeshEditor::handleToolChange(State::Tool tool) {
    switch (tool) {
    case State::Tool::Draw:
        _tool = makeShared<DrawTool>(_appState, _object, _mesh);
        break;
    case State::Tool::Extrude:
        _tool = makeShared<ExtrudeTool>(_appState, _object, _mesh);
        break;
    case State::Tool::LoopCut:
        _tool = makeShared<LoopCutTool>(_appState, _object, _mesh);
        break;
    case State::Tool::BorderSelect:
        _tool = makeShared<BorderSelectTool>(_appState, _object, _mesh);
        break;
    default:
        _tool = makeShared<MoveTool>(_appState, _object, _mesh);
        break;
    }
    connect(_tool.get(), &Tool::meshChanged, this, [this] {
        _isVAOsDirty = true;
        emit updated();
    });
    connect(_tool.get(), &Tool::meshChangeFinished, this, [this] (const QString& title) {
        _appState->document()->history()->beginChange(title);
        _object->setMesh(*_mesh);
        _appState->setTool(State::Tool::None);
    });
    updateChildren();
}

void MeshEditor::updateManinpulatorVisibility() {
    _manipulator->setVisible(!_appState->document()->meshSelection().empty() && _appState->tool() == State::Tool::None);
}

void MeshEditor::updateChildren() {
    auto children = _pickables;
    children.push_back(_manipulator);
    children.push_back(_tool);
    setChildRenderables(children);
}

void MeshEditor::mousePressTarget(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    _tool->mousePressTool(target, event);
}

void MeshEditor::mouseMoveTarget(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    _tool->mouseMoveTool(target, event);
}

void MeshEditor::mouseReleaseTarget(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    _tool->mouseReleaseTool(target, event);
}

void MeshEditor::hoverEnterTarget(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    Q_UNUSED(event);
    // TODO: update partially
    if (target.vertex) {
        _hoveredVertex = target.vertex;
        _isVAOsDirty = true;
        emit updated();
    } else if (target.edge) {
        _hoveredEdge = target.edge;
        _isVAOsDirty = true;
        emit updated();
    } else if (target.face) {
        _hoveredFace = target.face;
        _isVAOsDirty = true;
        emit updated();
    }
    _tool->hoverEnterTool(target, event);
}

void MeshEditor::hoverLeaveTarget(const Tool::EventTarget &target) {
    // TODO: update partially
    if (target.vertex) {
        _hoveredVertex = {};
        _isVAOsDirty = true;
        emit updated();
    } else if (target.edge) {
        _hoveredEdge = {};
        _isVAOsDirty = true;
        emit updated();
    } else if (target.face) {
        _hoveredFace = {};
        _isVAOsDirty = true;
        emit updated();
    }
    _tool->hoverLeaveTool(target);
}

void MeshEditor::contextMenuTarget(const Tool::EventTarget &target, const Viewport::ContextMenuEvent &event) {
    Q_UNUSED(target);

    QMenu contextMenu;
    contextMenu.addAction(tr("Delete Vertices"), _appState.get(), &State::AppState::deleteVertices);
    contextMenu.addAction(tr("Delete Edges"), _appState.get(), &State::AppState::deleteEdges);
    contextMenu.addAction(tr("Delete Faces"), _appState.get(), &State::AppState::deleteFaces);
    contextMenu.exec(event.originalEvent->globalPos());
}

void MeshEditor::updateVAOs() {
    auto& mesh = *_mesh;
    auto hitTestExclusion = _tool->hitTestExclusion();

    _pickables.clear();

    {
        std::vector<Draw::PointLineVertex> vertexAttributes;
        std::vector<Draw::PointLineVertex> vertexPickAttributes;
        std::vector<SP<VertexPickable>> vertexPickables;

        vertexAttributes.reserve(mesh.vertexCount());
        vertexPickAttributes.reserve(mesh.vertexCount());

        for (auto v : mesh.vertices()) {
            bool selected = mesh.isSelected(v);
            bool hovered = v == _hoveredVertex;
            bool hitTestExcluded = ranges::find(hitTestExclusion.vertices, v) != hitTestExclusion.vertices.end();

            Draw::PointLineVertex attrib;
            attrib.position = mesh.position(v);
            attrib.color = hovered ? hoveredColor : selected ? selectedColor : unselectedColor;
            vertexAttributes.push_back(attrib);

            auto pickable = makeShared<VertexPickable>(this, v);
            _pickables.push_back(pickable);

            Draw::PointLineVertex pickAttrib;
            pickAttrib.position = mesh.position(v);
            pickAttrib.color = pickable->toIDColor();
            pickAttrib.width = hitTestExcluded ? 0 : 1;
            vertexPickAttributes.push_back(pickAttrib);
        }

        _vertexVBO->setVertices(vertexAttributes);
        _vertexPickVBO->setVertices(vertexPickAttributes);
    }

    {
        std::vector<Draw::PointLineVertex> edgeAttributes;
        std::vector<Draw::PointLineVertex> edgePickAttributes;

        edgeAttributes.reserve(mesh.edgeCount() * 2);
        edgePickAttributes.reserve(mesh.edgeCount() * 2);

        for (auto e : mesh.edges()) {
            bool hovered = e == _hoveredEdge;
            bool hitTestExcluded = ranges::find(hitTestExclusion.edges, e) != hitTestExclusion.edges.end();

            auto pickable = makeShared<EdgePickable>(this, e);
            _pickables.push_back(pickable);

            for (size_t vertexInEdgeIndex = 0; vertexInEdgeIndex < 2; ++vertexInEdgeIndex) {
                auto& v = mesh.vertices(e)[vertexInEdgeIndex];
                bool selected = mesh.isSelected(v);

                Draw::PointLineVertex attrib;
                attrib.position = mesh.position(v);
                attrib.color = hovered ? hoveredColor : selected ? selectedColor : unselectedColor;
                edgeAttributes.push_back(attrib);

                Draw::PointLineVertex pickAttrib;
                pickAttrib.position = mesh.position(v);
                pickAttrib.color = pickable->toIDColor();
                pickAttrib.width = hitTestExcluded ? 0 : 1;
                edgePickAttributes.push_back(pickAttrib);
            }
        }

        _edgeVBO->setVertices(edgeAttributes);
        _edgePickVBO->setVertices(edgePickAttributes);
    }

    {
        std::vector<Draw::Vertex> facePickAttributes;
        std::vector<GL::IndexBuffer::Triangle> faceTriangles;

        for (auto f : mesh.faces()) {
            auto pickable = makeShared<FacePickable>(this, f);
            _pickables.push_back(pickable);

            auto idColor = pickable->toIDColor();

            auto i0 = uint32_t(facePickAttributes.size());
            for (uint32_t i = 2; i < uint32_t(mesh.vertices(f).size()); ++i) {
                auto i1 = i0 + i - 1;
                auto i2 = i0 + i;
                faceTriangles.push_back({i0, i1, i2});
            }

            for (auto& p : mesh.uvPoints(f)) {
                Draw::Vertex pickAttrib;
                pickAttrib.position = mesh.position(mesh.vertex(p));
                pickAttrib.color = idColor;
                facePickAttributes.push_back(pickAttrib);
            }
        }
        _facePickVBO->setVertices(facePickAttributes);
        _faceIBO->setTriangles(faceTriangles);
    }

    updateChildren();
}

}
