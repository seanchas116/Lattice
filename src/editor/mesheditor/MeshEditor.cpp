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

class MeshEditor::VertexPickable : public Render::Renderable {
public:
    VertexPickable(MeshEditor* editor, const SP<Mesh::Vertex>& vertex) : _editor(editor), _vertex(vertex) {}

    void mousePress(const Render::MouseEvent &event) override {
        _editor->mousePressTarget({_vertex, {}, {}}, event);
    }

    void mouseMove(const Render::MouseEvent &event) override {
        _editor->mouseMoveTarget({_vertex, {}, {}}, event);
    }

    void mouseRelease(const Render::MouseEvent &event) override {
        _editor->mouseReleaseTarget({_vertex, {}, {}}, event);
    }

    void hoverEnter(const Render::MouseEvent &event) override {
        _editor->hoverEnterTarget({_vertex, {}, {}}, event);
    }

    void hoverLeave() override {
        _editor->hoverLeaveTarget({_vertex, {}, {}});
    }

private:
    MeshEditor* _editor;
    SP<Mesh::Vertex> _vertex;
};

class MeshEditor::EdgePickable : public Render::Renderable {
public:
    EdgePickable(MeshEditor* editor, const SP<Mesh::Edge>& edge) : _editor(editor), _edge(edge) {}

    void mousePress(const Render::MouseEvent &event) override {
        _editor->mousePressTarget({{}, _edge, {}}, event);
    }

    void mouseMove(const Render::MouseEvent &event) override {
        _editor->mouseMoveTarget({{}, _edge, {}}, event);
    }

    void mouseRelease(const Render::MouseEvent &event) override {
        _editor->mouseReleaseTarget({{}, _edge, {}}, event);
    }

    void hoverEnter(const Render::MouseEvent &event) override {
        _editor->hoverEnterTarget({{}, _edge, {}}, event);
    }

    void hoverLeave() override {
        _editor->hoverLeaveTarget({{}, _edge, {}});
    }

private:
    MeshEditor* _editor;
    SP<Mesh::Edge> _edge;
};

class MeshEditor::FacePickable : public Render::Renderable {
public:
    FacePickable(MeshEditor* editor, const SP<Mesh::Face>& face) : _editor(editor), _face(face) {}

    void mousePress(const Render::MouseEvent &event) override {
        _editor->mousePressTarget({{}, {}, _face}, event);
    }

    void mouseMove(const Render::MouseEvent &event) override {
        _editor->mouseMoveTarget({{}, {}, _face}, event);
    }

    void mouseRelease(const Render::MouseEvent &event) override {
        _editor->mouseReleaseTarget({{}, {}, _face}, event);
    }

    void hoverEnter(const Render::MouseEvent &event) override {
        _editor->hoverEnterTarget({{}, {}, _face}, event);
    }

    void hoverLeave() override {
        _editor->hoverLeaveTarget({{}, {}, _face});
    }

private:
    MeshEditor* _editor;
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
    connect(_item->mesh().get(), &Mesh::Mesh::changed, this, &MeshEditor::updateWholeVAOs);
    connect(_appState->document().get(), &Document::Document::meshSelectionChanged, this, &MeshEditor::updateWholeVAOs);
    connect(_appState.get(), &State::AppState::toolChanged, this, &MeshEditor::handleToolChange);
}

void MeshEditor::draw(const SP<Render::Operations> &operations, const SP<Camera> &camera) {
    if (_appState->isFaceVisible()) {
        for (auto& [material, vao] : _faceVAOs) {
            operations->drawMaterial.draw(vao, _item->location().matrixToWorld(), camera, material);
        }
    }
    if (_appState->isEdgeVisible()) {
        operations->drawLine.draw(_edgeVAO, _item->location().matrixToWorld(), camera, 1.0, vec4(0), true);
    }
    if (_appState->isVertexVisible()) {
        operations->drawCircle.draw(_vertexVAO, _item->location().matrixToWorld(), camera, 6.0, vec4(0), true);
    }
}

void MeshEditor::drawPickables(const SP<Render::Operations> &operations, const SP<Camera> &camera) {
    auto idColor = toIDColor();
    glClearColor(idColor.r, idColor.g, idColor.b, idColor.a);
    glClearDepthf(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (_appState->isFaceVisible()) {
        operations->drawUnicolor.draw(_facePickVAO, _item->location().matrixToWorld(), camera, vec4(0), true);
    }
    if (_appState->isEdgeVisible()) {
        operations->drawLine.draw(_edgePickVAO, _item->location().matrixToWorld(), camera, 12.0, vec4(0), true);
    }
    if (_appState->isVertexVisible()) {
        operations->drawCircle.draw(_vertexPickVAO, _item->location().matrixToWorld(), camera, 24.0, vec4(0), true);
    }
}

void MeshEditor::mousePress(const Render::MouseEvent &event) {
    mousePressTarget({}, event);
}

void MeshEditor::mouseMove(const Render::MouseEvent &event) {
    mouseMoveTarget({}, event);
}

void MeshEditor::mouseRelease(const Render::MouseEvent &event) {
    mouseReleaseTarget({}, event);
}

void MeshEditor::keyPressEvent(QKeyEvent *event) {
    qDebug() << "key press" << event->key();
}

void MeshEditor::keyReleaseEvent(QKeyEvent *event) {
    qDebug() << "key release" << event->key();
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

void MeshEditor::updateWholeVAOs() {
    recallContext();

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

        auto addPoint = [&](const SP<FacePickable>& pickable, const SP<Mesh::UVPoint>& p, bool isBack) {
            GL::Vertex attrib;
            attrib.position = p->vertex()->position();
            attrib.texCoord = p->position();
            attrib.normal = isBack ? -p->vertex()->normal() : p->vertex()->normal();

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

                { // fore
                    auto i0 = addPoint(pickable, face->uvPoints()[0], false);
                    for (uint32_t i = 2; i < uint32_t(face->vertices().size()); ++i) {
                        auto i1 = addPoint(pickable, face->uvPoints()[i - 1], false);
                        auto i2 = addPoint(pickable, face->uvPoints()[i], false);
                        triangles.push_back({i0, i1, i2});
                        pickTriangles.push_back({i0, i1, i2});
                    }
                }

                /*
                { // back
                    auto i0 = addPoint(pickable, face->uvPoints()[0], true);
                    for (uint32_t i = 2; i < uint32_t(face->vertices().size()); ++i) {
                        auto i1 = addPoint(pickable, face->uvPoints()[i - 1], true);
                        auto i2 = addPoint(pickable, face->uvPoints()[i], true);
                        triangles.push_back({i0, i2, i1});
                        pickTriangles.push_back({i0, i2, i1});
                    }
                }
                */
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

    setChildren(childPickables);
    update();
}

void MeshEditor::mousePressTarget(const Tool::EventTarget &target, const Render::MouseEvent &event) {
    if (event.originalEvent->button() != Qt::LeftButton) {
        if (event.originalEvent->button() == Qt::RightButton) {
            QMenu contextMenu;
            contextMenu.addAction(tr("Delete Vertices"), _appState.get(), &State::AppState::deleteVertices);
            contextMenu.addAction(tr("Delete Edges"), _appState.get(), &State::AppState::deleteEdges);
            contextMenu.addAction(tr("Delete Faces"), _appState.get(), &State::AppState::deleteFaces);
            contextMenu.exec(event.originalEvent->globalPos());
        }
        return;
    }

    _tool->mousePress(target, event);
}

void MeshEditor::mouseMoveTarget(const Tool::EventTarget &target, const Render::MouseEvent &event) {
    _tool->mouseMove(target, event);
}

void MeshEditor::mouseReleaseTarget(const Tool::EventTarget &target, const Render::MouseEvent &event) {
    _tool->mouseRelease(target, event);
}

void MeshEditor::hoverEnterTarget(const Tool::EventTarget &target, const Render::MouseEvent &event) {
    Q_UNUSED(event);
    if (target.vertex) {
        _hoveredVertex = target.vertex;
        updateWholeVAOs(); // TODO: update partially
    } else if (target.edge) {
        _hoveredEdge = target.edge;
        updateWholeVAOs(); // TODO: update partially
    }
    _tool->hoverEnter(target, event);
}

void MeshEditor::hoverLeaveTarget(const Tool::EventTarget &target) {
    if (target.vertex) {
        _hoveredVertex = {};
        updateWholeVAOs(); // TODO: update partially
    } else if (target.edge) {
        _hoveredEdge = {};
        updateWholeVAOs(); // TODO: update partially
    }
    _tool->hoverLeave(target);
}

}
