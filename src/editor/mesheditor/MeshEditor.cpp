#include "MeshEditor.hpp"
#include "MoveTool.hpp"
#include "DrawTool.hpp"
#include "ExtrudeTool.hpp"
#include "LoopCutTool.hpp"
#include "../../state/AppState.hpp"
#include "../../gl/VAO.hpp"
#include "../../gl/VertexBuffer.hpp"
#include "../../document/Document.hpp"
#include "../../document/History.hpp"
#include "../../document/MeshItem.hpp"
#include "../../mesh/Mesh.hpp"
#include "../../support/Debug.hpp"
#include "../../support/Camera.hpp"
#include <QMouseEvent>
#include <QMenu>

using namespace glm;

namespace Lattice::Editor::MeshEditor {

namespace {

const vec4 unselectedColor = vec4(0, 0, 0, 1);
const vec4 selectedColor = vec4(1, 1, 1, 1);
const vec4 hoveredColor = vec4(1, 1, 0, 1);

}

class MeshEditor::EditorPickable : public Render::Renderable {
public:
    EditorPickable(MeshEditor* editor) : _editor(editor) {}
    void mousePressEvent(const Render::MouseEvent &event) override {
        _editor->mousePressTarget(target(), event);
    }
    void mouseMoveEvent(const Render::MouseEvent &event) override {
        _editor->mouseMoveTarget(target(), event);
    }
    void mouseReleaseEvent(const Render::MouseEvent &event) override {
        _editor->mouseReleaseTarget(target(), event);
    }
    void contextMenuEvent(const Render::ContextMenuEvent &event) override {
        _editor->contextMenuTarget(target(), event);
    }
    void hoverEnterEvent(const Render::MouseEvent &event) override {
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
    VertexPickable(MeshEditor* editor, const SP<Mesh::Vertex>& vertex) : EditorPickable(editor), _vertex(vertex) {}
    Tool::EventTarget target() const override {
        return {_vertex, {}, {}};
    }
private:
    SP<Mesh::Vertex> _vertex;
};

class MeshEditor::EdgePickable : public MeshEditor::EditorPickable {
public:
    EdgePickable(MeshEditor* editor, const SP<Mesh::Edge>& edge) : EditorPickable(editor), _edge(edge) {}
    Tool::EventTarget target() const override {
        return {{}, _edge, {}};
    }
private:
    SP<Mesh::Edge> _edge;
};

class MeshEditor::FacePickable : public MeshEditor::EditorPickable {
public:
    FacePickable(MeshEditor* editor, const SP<Mesh::Face>& face) : EditorPickable(editor), _face(face) {}
    Tool::EventTarget target() const override {
        return {{}, {}, _face};
    }
private:
    SP<Mesh::Face> _face;
};

MeshEditor::MeshEditor(const SP<State::AppState>& appState, const SP<Document::MeshItem> &item) :
    _appState(appState),
    _item(item),
    _faceVBO(makeShared<GL::VertexBuffer<GL::Vertex>>()),
    _facePickVAO(makeShared<GL::VAO>()),
    _edgeVAO(makeShared<GL::VAO>()),
    _edgePickVAO(makeShared<GL::VAO>()),
    _vertexVAO(makeShared<GL::VAO>()),
    _vertexPickVAO(makeShared<GL::VAO>()),
    _tool(makeShared<MoveTool>(appState, item))
{
    initializeOpenGLFunctions();
    updateWholeVAOs();
    connect(_item->mesh().get(), &Mesh::Mesh::changed, this, &MeshEditor::handleMeshChange);
    connect(_appState->document().get(), &Document::Document::meshSelectionChanged, this, &MeshEditor::handleMeshChange);
    connect(_appState.get(), &State::AppState::toolChanged, this, &MeshEditor::handleToolChange);
}

void MeshEditor::draw(const SP<Render::Operations> &operations, const SP<Camera> &camera) {
    updateWholeVAOs();

    for (auto& [material, vao] : _faceVAOs) {
        operations->drawMaterial.draw(vao, _item->location().matrixToWorld(), camera, material);
    }
    operations->drawLine.draw(_edgeVAO, _item->location().matrixToWorld(), camera, 1.0, vec4(0), true);
    if (_appState->isVertexSelectable()) {
        operations->drawCircle.draw(_vertexVAO, _item->location().matrixToWorld(), camera, 6.0, vec4(0), true);
    }
}

void MeshEditor::drawPickables(const SP<Render::Operations> &operations, const SP<Camera> &camera) {
    updateWholeVAOs();

    auto idColor = toIDColor();
    glClearColor(idColor.r, idColor.g, idColor.b, idColor.a);
    glClearDepthf(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (_appState->isFaceSelectable()) {
        operations->drawUnicolor.draw(_facePickVAO, _item->location().matrixToWorld(), camera, vec4(0), true);
    }
    if (_appState->isEdgeSelectable()) {
        operations->drawLine.draw(_edgePickVAO, _item->location().matrixToWorld(), camera, 12.0, vec4(0), true);
    }
    if (_appState->isVertexSelectable()) {
        operations->drawCircle.draw(_vertexPickVAO, _item->location().matrixToWorld(), camera, 24.0, vec4(0), true);
    }
}

void MeshEditor::mousePressEvent(const Render::MouseEvent &event) {
    mousePressTarget({}, event);
}

void MeshEditor::mouseMoveEvent(const Render::MouseEvent &event) {
    mouseMoveTarget({}, event);
}

void MeshEditor::mouseReleaseEvent(const Render::MouseEvent &event) {
    mouseReleaseTarget({}, event);
}

void MeshEditor::contextMenuEvent(const Render::ContextMenuEvent &event) {
    contextMenuTarget({}, event);
}

void MeshEditor::keyPressEvent(QKeyEvent *event) {
    _tool->keyPressEvent(event);
}

void MeshEditor::keyReleaseEvent(QKeyEvent *event) {
    _tool->keyReleaseEvent(event);
}

void MeshEditor::handleToolChange(State::Tool tool) {
    switch (tool) {
    case State::Tool::Draw:
        _tool = makeShared<DrawTool>(_appState, _item);
        break;
    case State::Tool::Extrude:
        _tool = makeShared<ExtrudeTool>(_appState, _item);
        break;
    case State::Tool::LoopCut:
        _tool = makeShared<LoopCutTool>(_appState, _item);
        break;
    default:
        _tool = makeShared<MoveTool>(_appState, _item);
        break;
    }
}

void MeshEditor::handleMeshChange() {
    _isVAOsDirty = true;
    update();
}

void MeshEditor::updateWholeVAOs() {
    if (!_isVAOsDirty) {
        return;
    }

    auto& selectedVertices = _appState->document()->meshSelection().vertices;

    auto hitTestExclusion = _tool->hitTestExclusion();

    std::vector<SP<Render::Renderable>> childPickables;

    {
        _vertexAttributes.clear();
        _vertexAttributes.reserve(_item->mesh()->vertices().size());
        _vertexPickAttributes.clear();
        _vertexPickAttributes.reserve(_item->mesh()->vertices().size());

        for (auto& v : _item->mesh()->vertices()) {
            bool selected = selectedVertices.find(v) != selectedVertices.end();
            bool hovered = v == _hoveredVertex;

            GL::Vertex attrib;
            attrib.position = v->position();
            attrib.color = hovered ? hoveredColor : selected ? selectedColor : unselectedColor;
            _vertexAttributes.push_back(attrib);

            bool hitTestExcluded = std::find(hitTestExclusion.vertices.begin(), hitTestExclusion.vertices.end(), v) != hitTestExclusion.vertices.end();

            if (!hitTestExcluded) {
                auto pickable = makeShared<VertexPickable>(this, v);
                childPickables.push_back(pickable);
                GL::Vertex pickAttrib;
                pickAttrib.position = v->position();
                pickAttrib.color = pickable->toIDColor();

                _vertexPickAttributes.push_back(pickAttrib);
            }
        }

        auto vertexBuffer = makeShared<GL::VertexBuffer<GL::Vertex>>();
        vertexBuffer->setVertices(_vertexAttributes);
        _vertexVAO = makeShared<GL::VAO>(vertexBuffer, GL::Primitive::Point);

        auto pickVertexBuffer = makeShared<GL::VertexBuffer<GL::Vertex>>();
        pickVertexBuffer->setVertices(_vertexPickAttributes);
        _vertexPickVAO = makeShared<GL::VAO>(pickVertexBuffer, GL::Primitive::Point);
    }

    {
        _edgeAttributes.clear();
        _edgeAttributes.reserve(_item->mesh()->edges().size() * 2);
        _edgePickAttributes.clear();
        _edgePickAttributes.reserve(_item->mesh()->edges().size() * 2);

        std::vector<GL::IndexBuffer::Line> indices;
        for (auto& [_, e] : _item->mesh()->edges()) {
            bool hovered = e == _hoveredEdge;

            auto pickable = makeShared<EdgePickable>(this, e);
            childPickables.push_back(pickable);

            auto offset = uint32_t(_edgeAttributes.size());

            bool hitTestExcluded = std::find(hitTestExclusion.edges.begin(), hitTestExclusion.edges.end(), e) != hitTestExclusion.edges.end();

            for (auto& v : e->vertices()) {
                bool selected = selectedVertices.find(v) != selectedVertices.end();

                GL::Vertex attrib;
                attrib.position = v->position();
                attrib.color = hovered ? hoveredColor : selected ? selectedColor : unselectedColor;
                _edgeAttributes.push_back(attrib);

                if (!hitTestExcluded) {
                    GL::Vertex pickAttrib;
                    pickAttrib.position = v->position();
                     pickAttrib.color = pickable->toIDColor();
                    _edgePickAttributes.push_back(pickAttrib);
                }
            }
            indices.push_back({offset, offset+1});
        }

        auto vertexBuffer = makeShared<GL::VertexBuffer<GL::Vertex>>();
        auto indexBuffer = makeShared<GL::IndexBuffer>();
        vertexBuffer->setVertices(_edgeAttributes);
        indexBuffer->setLines(indices);
        _edgeVAO = makeShared<GL::VAO>(vertexBuffer, indexBuffer);

        auto pickVertexBuffer = makeShared<GL::VertexBuffer<GL::Vertex>>();
        pickVertexBuffer->setVertices(_edgePickAttributes);
        _edgePickVAO = makeShared<GL::VAO>(pickVertexBuffer, indexBuffer);
    }

    {
        _faceVBO = makeShared<GL::VertexBuffer<GL::Vertex>>();

        _faceVAOs.clear();
        _faceAttributes.clear();
        _facePickAttributes.clear();

        auto addPoint = [&](const SP<Mesh::Face>& face, const SP<FacePickable>& pickable, const SP<Mesh::UVPoint>& p) {
            GL::Vertex attrib;
            attrib.position = p->vertex()->position();
            attrib.texCoord = p->position();
            attrib.normal = p->vertex()->normal(face);

            GL::Vertex pickAttrib;
            pickAttrib.position = p->vertex()->position();
            pickAttrib.color = pickable->toIDColor();

            auto index = uint32_t(_faceAttributes.size());
            _faceAttributes.push_back(attrib);
            _facePickAttributes.push_back(pickAttrib);
            return index;
        };

        std::vector<GL::IndexBuffer::Triangle> pickTriangles;

        for (auto& material : _item->mesh()->materials()) {
            std::vector<GL::IndexBuffer::Triangle> triangles;
            for (auto& face : material->faces()) {
                auto pickable = makeShared<FacePickable>(this, face->sharedFromThis());
                childPickables.push_back(pickable);

                auto i0 = addPoint(face->sharedFromThis(), pickable, face->uvPoints()[0]);
                for (uint32_t i = 2; i < uint32_t(face->vertices().size()); ++i) {
                    auto i1 = addPoint(face->sharedFromThis(), pickable, face->uvPoints()[i - 1]);
                    auto i2 = addPoint(face->sharedFromThis(), pickable, face->uvPoints()[i]);
                    triangles.push_back({i0, i1, i2});
                    pickTriangles.push_back({i0, i1, i2});
                }
            }
            auto indexBuffer = makeShared<GL::IndexBuffer>();
            indexBuffer->setTriangles(triangles);
            auto vao = makeShared<GL::VAO>(_faceVBO, indexBuffer);
            _faceVAOs.insert({material, vao});
        }
        _faceVBO->setVertices(_faceAttributes);

        auto pickIndexBuffer = makeShared<GL::IndexBuffer>();
        pickIndexBuffer->setTriangles(pickTriangles);
        auto pickVertexBuffer = makeShared<GL::VertexBuffer<GL::Vertex>>();
        pickVertexBuffer->setVertices(_facePickAttributes);
        _facePickVAO = makeShared<GL::VAO>(pickVertexBuffer, pickIndexBuffer);
    }

    setChildRenderables(childPickables);

    _isVAOsDirty = false;
}

void MeshEditor::mousePressTarget(const Tool::EventTarget &target, const Render::MouseEvent &event) {
    _tool->mousePressEvent(target, event);
}

void MeshEditor::mouseMoveTarget(const Tool::EventTarget &target, const Render::MouseEvent &event) {
    _tool->mouseMoveEvent(target, event);
}

void MeshEditor::mouseReleaseTarget(const Tool::EventTarget &target, const Render::MouseEvent &event) {
    _tool->mouseReleaseEvent(target, event);
}

void MeshEditor::hoverEnterTarget(const Tool::EventTarget &target, const Render::MouseEvent &event) {
    Q_UNUSED(event);
    // TODO: update partially
    if (target.vertex) {
        _hoveredVertex = target.vertex;
        _isVAOsDirty = true;
        update();
    } else if (target.edge) {
        _hoveredEdge = target.edge;
        _isVAOsDirty = true;
        update();
    }
    _tool->hoverEnterEvent(target, event);
}

void MeshEditor::hoverLeaveTarget(const Tool::EventTarget &target) {
    // TODO: update partially
    if (target.vertex) {
        _hoveredVertex = {};
        _isVAOsDirty = true;
        update();
    } else if (target.edge) {
        _hoveredEdge = {};
        _isVAOsDirty = true;
        update();
    }
    _tool->hoverLeaveEvent(target);
}

void MeshEditor::contextMenuTarget(const Tool::EventTarget &target, const Render::ContextMenuEvent &event) {
    Q_UNUSED(target);

    QMenu contextMenu;
    contextMenu.addAction(tr("Delete Vertices"), _appState.get(), &State::AppState::deleteVertices);
    contextMenu.addAction(tr("Delete Edges"), _appState.get(), &State::AppState::deleteEdges);
    contextMenu.addAction(tr("Delete Faces"), _appState.get(), &State::AppState::deleteFaces);
    contextMenu.exec(event.originalEvent->globalPos());
}

}
