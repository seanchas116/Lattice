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
    VertexPickable(MeshEditor* editor, const SP<OldMesh::Vertex>& vertex) : EditorPickable(editor), _vertex(vertex) {}
    Tool::EventTarget target() const override {
        return {_vertex, {}, {}};
    }
private:
    SP<OldMesh::Vertex> _vertex;
};

class MeshEditor::EdgePickable : public MeshEditor::EditorPickable {
public:
    EdgePickable(MeshEditor* editor, const SP<OldMesh::Edge>& edge) : EditorPickable(editor), _edge(edge) {}
    Tool::EventTarget target() const override {
        return {{}, _edge, {}};
    }
private:
    SP<OldMesh::Edge> _edge;
};

class MeshEditor::FacePickable : public MeshEditor::EditorPickable {
public:
    FacePickable(MeshEditor* editor, const SP<OldMesh::Face>& face) : EditorPickable(editor), _face(face) {}
    Tool::EventTarget target() const override {
        return {{}, {}, _face};
    }
private:
    SP<OldMesh::Face> _face;
};

MeshEditor::MeshEditor(const SP<State::AppState>& appState, const SP<Document::MeshObject> &object) :
    _appState(appState),
    _object(object),
    _mesh(makeShared<Mesh::Mesh>(object->mesh())),
    _manipulator(makeShared<Manipulator::MeshManipulator>(appState, object)),

    _faceIndexBuffer(makeShared<GL::IndexBuffer>()),
    _faceVertexBuffer(makeShared<GL::VertexBuffer<Draw::Vertex>>()),
    _faceVAO(makeShared<GL::VAO>(_faceVertexBuffer, _faceIndexBuffer)),
    _facePickVertexBuffer(makeShared<GL::VertexBuffer<Draw::Vertex>>()),
    _facePickVAO(makeShared<GL::VAO>(_facePickVertexBuffer, _faceIndexBuffer)),

    _edgeVertexBuffer(makeShared<GL::VertexBuffer<Draw::PointLineVertex>>()),
    _edgeVAO(makeShared<GL::VAO>(_edgeVertexBuffer, GL::Primitive::Line)),
    _edgePickVertexBuffer(makeShared<GL::VertexBuffer<Draw::PointLineVertex>>()),
    _edgePickVAO(makeShared<GL::VAO>(_edgePickVertexBuffer, GL::Primitive::Line)),

    _vertexVertexBuffer(makeShared<GL::VertexBuffer<Draw::PointLineVertex>>()),
    _vertexVAO(makeShared<GL::VAO>(_vertexVertexBuffer, GL::Primitive::Point)),
    _vertexPickVertexBuffer(makeShared<GL::VertexBuffer<Draw::PointLineVertex>>()),
    _vertexPickVAO(makeShared<GL::VAO>(_vertexPickVertexBuffer, GL::Primitive::Point)),

    _tool(makeShared<MoveTool>(appState, object))
{
    initializeOpenGLFunctions();
    updateWholeVAOs();

    connect(object->oldMesh().get(), &OldMesh::Mesh::topologyChanged, this, &MeshEditor::handleMeshTopologyChange);
    connect(object->oldMesh().get(), &OldMesh::Mesh::changed, this, &MeshEditor::handleMeshChange);
    connect(appState->document().get(), &Document::Document::meshSelectionChanged, this, &MeshEditor::handleMeshChange);

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
    updateWholeVAOs();

    // TODO: Render faces

    operations->drawLine.draw(_edgeVAO, _object->location().matrixToWorld(), camera, 1.0, vec4(0), true);
    if (_appState->isVertexSelectable()) {
        operations->drawCircle.draw(_vertexVAO, _object->location().matrixToWorld(), camera, 6.0, vec4(0), true);
    }
}

void MeshEditor::drawPickables(const SP<Draw::Operations> &operations, const SP<Camera> &camera) {
    updateWholeVAOs();

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
        _tool = makeShared<DrawTool>(_appState, _object);
        break;
    case State::Tool::Extrude:
        _tool = makeShared<ExtrudeTool>(_appState, _object);
        break;
    case State::Tool::LoopCut:
        _tool = makeShared<LoopCutTool>(_appState, _object);
        break;
    case State::Tool::BorderSelect:
        _tool = makeShared<BorderSelectTool>(_appState, _object);
        break;
    default:
        _tool = makeShared<MoveTool>(_appState, _object);
        break;
    }
    connect(_tool.get(), &Tool::finished, _appState.get(), [appState = _appState] {
        appState->setTool(State::Tool::None);
    });
    updateChildren();
}

void MeshEditor::handleMeshTopologyChange() {
    _isVAOTopologyDirty = true;
    emit updated();
}

void MeshEditor::handleMeshChange() {
    _isVAOAttributesDirty = true;
    emit updated();
}

void MeshEditor::updateWholeVAOs() {
    updateVAOTopology();
    updateVAOAttributes();
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
        _isVAOAttributesDirty = true;
        emit updated();
    } else if (target.edge) {
        _hoveredEdge = target.edge;
        _isVAOAttributesDirty = true;
        emit updated();
    } else if (target.face) {
        _hoveredFace = target.face;
        _isVAOAttributesDirty = true;
        emit updated();
    }
    _tool->hoverEnterTool(target, event);
}

void MeshEditor::hoverLeaveTarget(const Tool::EventTarget &target) {
    // TODO: update partially
    if (target.vertex) {
        _hoveredVertex = {};
        _isVAOAttributesDirty = true;
        emit updated();
    } else if (target.edge) {
        _hoveredEdge = {};
        _isVAOAttributesDirty = true;
        emit updated();
    } else if (target.face) {
        _hoveredFace = {};
        _isVAOAttributesDirty = true;
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

void MeshEditor::updateVAOTopology() {
    if (!_isVAOTopologyDirty) {
        return;
    }
    _isVAOTopologyDirty = false;
    _isVAOAttributesDirty = true;

    auto& mesh = _object->oldMesh();

    auto hitTestExclusion = _tool->hitTestExclusion();

    {
        _facePickables.clear();
        _vertexAttributes.resize(mesh->vertices().size());
        _vertexPickAttributes.resize(mesh->vertices().size());

        _vertices.clear();
        _vertices.reserve(mesh->vertices().size());
        _vertexPickables.clear();
        _vertexPickables.reserve(mesh->vertices().size());

        for (auto& v : mesh->vertices()) {
            _vertices.push_back(v);
            auto pickable = makeShared<VertexPickable>(this, v);
            _vertexPickables.push_back(pickable);
        }
    }

    {
        _edgeAttributes.resize(mesh->edges().size() * 2);
        _edgePickAttributes.resize(mesh->edges().size() * 2);

        _edges.clear();
        _edges.reserve(mesh->edges().size());
        _edgePickables.clear();
        _edgePickables.reserve(mesh->edges().size());

        for (auto& [_, e] : mesh->edges()) {
            _edges.push_back(e);
            auto pickable = makeShared<EdgePickable>(this, e);
            _edgePickables.push_back(pickable);
        }
    }

    {
        _faces.clear();
        _faces.reserve(mesh->edges().size());
        _facePickables.clear();
        _facePickables.reserve(mesh->edges().size());

        std::vector<GL::IndexBuffer::Triangle> triangles;
        uint32_t vertexCount = 0;

        for (auto& [_, face] : mesh->faces()) {
            _faces.push_back(face);

            auto pickable = makeShared<FacePickable>(this, face);
            _facePickables.push_back(pickable);

            auto i0 = vertexCount;
            for (uint32_t i = 2; i < uint32_t(face->vertices().size()); ++i) {
                auto i1 = vertexCount + i - 1;
                auto i2 = vertexCount + i;
                triangles.push_back({i0, i1, i2});
            }
            vertexCount += face->vertices().size();
        }

        _faceAttributes.resize(vertexCount);
        _facePickAttributes.resize(vertexCount);

        _faceIndexBuffer->setTriangles(triangles);
    }

    _pickables.clear();
    _pickables.insert(_pickables.end(), _vertexPickables.begin(), _vertexPickables.end());
    _pickables.insert(_pickables.end(), _edgePickables.begin(), _edgePickables.end());
    _pickables.insert(_pickables.end(), _facePickables.begin(), _facePickables.end());

    updateChildren();
}

void MeshEditor::updateVAOAttributes() {
    if (!_isVAOAttributesDirty) {
        return;
    }
    _isVAOAttributesDirty = false;

    auto& selectedVertices = _appState->document()->meshSelection().vertices;
    auto selectedFaces = _appState->document()->meshSelection().faces();

    auto hitTestExclusion = _tool->hitTestExclusion();

    for (size_t i = 0; i < _vertices.size(); ++i) {
        auto& v = _vertices[i];
        bool selected = selectedVertices.find(v) != selectedVertices.end();
        bool hovered = v == _hoveredVertex;
        bool hitTestExcluded = std::find(hitTestExclusion.vertices.begin(), hitTestExclusion.vertices.end(), v) != hitTestExclusion.vertices.end();

        Draw::PointLineVertex attrib;
        attrib.position = v->position();
        attrib.color = hovered ? hoveredColor : selected ? selectedColor : unselectedColor;
        _vertexAttributes[i] = attrib;

        Draw::PointLineVertex pickAttrib;
        pickAttrib.position = v->position();
        pickAttrib.color = _vertexPickables[i]->toIDColor();
        pickAttrib.width = hitTestExcluded ? 0 : 1;
        _vertexPickAttributes[i] = pickAttrib;
    }
    _vertexVertexBuffer->setVertices(_vertexAttributes);
    _vertexPickVertexBuffer->setVertices(_vertexPickAttributes);

    for (size_t edgeIndex = 0; edgeIndex < _edges.size(); ++edgeIndex) {
        auto& e = _edges[edgeIndex];
        bool hovered = e == _hoveredEdge;
        bool hitTestExcluded = std::find(hitTestExclusion.edges.begin(), hitTestExclusion.edges.end(), e) != hitTestExclusion.edges.end();

        for (size_t vertexInEdgeIndex = 0; vertexInEdgeIndex < 2; ++vertexInEdgeIndex) {
            auto& v = e->vertices()[vertexInEdgeIndex];
            bool selected = selectedVertices.find(v) != selectedVertices.end();

            Draw::PointLineVertex attrib;
            attrib.position = v->position();
            attrib.color = hovered ? hoveredColor : selected ? selectedColor : unselectedColor;
            _edgeAttributes[edgeIndex * 2 + vertexInEdgeIndex] = attrib;

            Draw::PointLineVertex pickAttrib;
            pickAttrib.position = v->position();
            pickAttrib.color = _edgePickables[edgeIndex]->toIDColor();
            pickAttrib.width = hitTestExcluded ? 0 : 1;
            _edgePickAttributes[edgeIndex * 2 + vertexInEdgeIndex] = pickAttrib;
        }
    }
    _edgeVertexBuffer->setVertices(_edgeAttributes);
    _edgePickVertexBuffer->setVertices(_edgePickAttributes);

    {
        size_t vertexIndex = 0;

        for (size_t faceIndex = 0; faceIndex < _faces.size(); ++faceIndex) {
            auto& face = _faces[faceIndex];

            bool hovered = _hoveredFace == face;
            bool selected = selectedFaces.find(face) != selectedFaces.end();

            auto idColor = _facePickables[faceIndex]->toIDColor();

            for (auto& p : face->uvPoints()) {
                Draw::Vertex attrib;
                attrib.position = p->vertex()->position();
                attrib.texCoord = p->position();
                attrib.color = hovered ? hoveredFaceHighlight : selected ? selectedFaceHighlight : unselectedFaceHighlight;

                Draw::Vertex pickAttrib;
                pickAttrib.position = p->vertex()->position();
                pickAttrib.color = idColor;

                _faceAttributes[vertexIndex] = attrib;
                _facePickAttributes[vertexIndex] = pickAttrib;
                ++vertexIndex;
            }
        }
        _faceVertexBuffer->setVertices(_faceAttributes);
        _facePickVertexBuffer->setVertices(_facePickAttributes);
    }
}

}
