#include "MeshEditor.hpp"
#include "MeshPicker.hpp"
#include "../ui/AppState.hpp"
#include "../gl/VAO.hpp"
#include "../gl/VertexBuffer.hpp"
#include "../document/Document.hpp"
#include "../document/History.hpp"
#include "../document/Mesh.hpp"
#include "../document/MeshItem.hpp"
#include "../support/Debug.hpp"
#include "../support/Camera.hpp"
#include <QMouseEvent>

using namespace glm;

namespace Lattice::Editor {

namespace {

const vec4 unselectedColor = vec4(0, 0, 0, 1);
const vec4 selectedColor = vec4(1, 1, 1, 1);
const vec4 hoveredColor = vec4(1, 1, 0, 1);

}

class MeshEditor::VertexPickable : public Render::Renderable {
public:
    VertexPickable(MeshEditor* editor, const SP<Document::MeshVertex>& vertex) : _editor(editor), _vertex(vertex) {}

    void mousePress(const Render::MouseEvent &event) override {
        _editor->vertexDragStart({_vertex}, event);
    }

    void mouseMove(const Render::MouseEvent &event) override {
        _editor->vertexDragMove(event);
    }

    void hoverEnter(const Render::MouseEvent &) override {
        _editor->_hoveredVertex = _vertex;
        _editor->updateWholeVAOs();
    }

    void hoverLeave() override {
        _editor->_hoveredVertex = {};
        _editor->updateWholeVAOs();
    }

    MeshEditor* _editor;
    SP<Document::MeshVertex> _vertex;
};

class MeshEditor::EdgePickable : public Render::Renderable {
public:
    EdgePickable(MeshEditor* editor, const SP<Document::MeshEdge>& edge) : _editor(editor), _edge(edge) {}

    void mousePress(const Render::MouseEvent &event) override {
        _editor->vertexDragStart({_edge->vertices()[0], _edge->vertices()[1]}, event);
    }

    void mouseMove(const Render::MouseEvent &event) override {
        _editor->vertexDragMove(event);
    }

    void hoverEnter(const Render::MouseEvent &) override {
        _editor->_hoveredEdge = _edge;
        _editor->updateWholeVAOs();
    }

    void hoverLeave() override {
        _editor->_hoveredEdge = {};
        _editor->updateWholeVAOs();
    }

    MeshEditor* _editor;
    SP<Document::MeshEdge> _edge;
};

class MeshEditor::FacePickable : public Render::Renderable {
public:
    FacePickable(MeshEditor* editor, const SP<Document::MeshFace>& face) : _editor(editor), _face(face) {}

    void mousePress(const Render::MouseEvent &event) override {
        std::unordered_set<SP<Document::MeshVertex>> vertices;
        for (auto& v : _face->vertices()) {
            vertices.insert(v);
        }
        _editor->vertexDragStart(vertices, event);
    }

    void mouseMove(const Render::MouseEvent &event) override {
        _editor->vertexDragMove(event);
    }

    void hoverEnter(const Render::MouseEvent &) override {
    }

    void hoverLeave() override {

    }

    MeshEditor* _editor;
    SP<Document::MeshFace> _face;
};

MeshEditor::MeshEditor(const SP<UI::AppState>& appState, const SP<Document::MeshItem> &item) :
    _appState(appState),
    _item(item),
    _faceVBO(makeShared<GL::VertexBuffer<GL::Vertex>>()),
    _facePickVAO(makeShared<GL::VAO>()),
    _edgeVAO(makeShared<GL::VAO>()),
    _edgePickVAO(makeShared<GL::VAO>()),
    _vertexVAO(makeShared<GL::VAO>()),
    _vertexPickVAO(makeShared<GL::VAO>())
{
    updateWholeVAOs();
    connect(_item->mesh().get(), &Document::Mesh::changed, this, &MeshEditor::updateWholeVAOs);
    connect(_appState->document().get(), &Document::Document::meshSelectionChanged, this, &MeshEditor::updateWholeVAOs);
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
    Q_UNUSED(event);
}

void MeshEditor::mouseMove(const Render::MouseEvent &event) {
    Q_UNUSED(event);
}

void MeshEditor::mouseRelease(const Render::MouseEvent &event) {
    Q_UNUSED(event);
}

void MeshEditor::hoverEnter(const Render::MouseEvent &event) {
    Q_UNUSED(event);
}

void MeshEditor::hoverMove(const Render::MouseEvent &event) {
    Q_UNUSED(event);
}

void MeshEditor::hoverLeave() {
}

void MeshEditor::updateWholeVAOs() {
    recallContext();

    auto& selectedVertices = _appState->document()->meshSelection().vertices;

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

            auto pickable = makeShared<VertexPickable>(this, v);
            childPickables.push_back(pickable);
            GL::Vertex pickAttrib;
            pickAttrib.position = v->position();
            pickAttrib.color = pickable->toIDColor();

            _vertexAttributes.push_back(attrib);
            _vertexPickAttributes.push_back(pickAttrib);
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
            for (auto& v : e->vertices()) {
                bool selected = selectedVertices.find(v) != selectedVertices.end();

                GL::Vertex attrib;
                attrib.position = v->position();
                attrib.color = hovered ? hoveredColor : selected ? selectedColor : unselectedColor;
                _edgeAttributes.push_back(attrib);

                GL::Vertex pickAttrib;
                pickAttrib.position = v->position();
                pickAttrib.color = pickable->toIDColor();
                _edgePickAttributes.push_back(pickAttrib);
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

        auto addPoint = [&](const SP<FacePickable>& pickable, const SP<Document::MeshUVPoint>& p) {
            GL::Vertex attrib;
            attrib.position = p->vertex()->position();
            attrib.texCoord = p->position();
            attrib.normal = p->vertex()->normal();

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

                auto i0 = addPoint(pickable, face->uvPoints()[0]);
                for (uint32_t i = 2; i < uint32_t(face->vertices().size()); ++i) {
                    auto i1 = addPoint(pickable, face->uvPoints()[i - 1]);
                    auto i2 = addPoint(pickable, face->uvPoints()[i]);
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

    setChildren(childPickables);
    updateRequested();
}

void MeshEditor::vertexDragStart(const std::unordered_set<SP<Document::MeshVertex> > &vertices, const Render::MouseEvent &event) {
    Document::MeshSelection selection;
    if (event.originalEvent->modifiers() & Qt::ShiftModifier) {
        selection = _appState->document()->meshSelection();

        bool alreadyAdded = true;
        for (auto& v : vertices) {
            if (selection.vertices.find(v) == selection.vertices.end()) {
                alreadyAdded = false;
            }
        }
        if (alreadyAdded) {
            for (auto& v : vertices) {
                selection.vertices.erase(v);
            }
        } else {
            for (auto& v: vertices) {
                selection.vertices.insert(v);
            }
        }
    } else {
        selection.vertices = vertices;
    }

    _dragInitPositions.clear();
    for (auto& v : selection.vertices) {
        _dragInitPositions[v] = v->position();
    }
    _dragInitWorldPos = event.worldPos();
    _dragStarted = false;

    _appState->document()->setMeshSelection(selection);
}

void MeshEditor::vertexDragMove(const Render::MouseEvent &event) {
    dvec3 worldPos = event.worldPos();
    dvec3 offset = worldPos - _dragInitWorldPos;

    if (!_dragStarted) {
        _appState->document()->history()->beginChange(tr("Move Vertex"));
        _dragStarted = true;
    }

    auto& mesh = _item->mesh();
    std::unordered_map<SP<Document::MeshVertex>, vec3> positions;
    for (auto& [v, initialPos] : _dragInitPositions) {
        positions[v] = initialPos + offset;
    }
    mesh->setPositions(positions);
}

}
