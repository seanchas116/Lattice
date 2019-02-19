#include "MeshEditor.hpp"
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
        _editor->mousePressTarget({.vertex = _vertex}, event);
    }

    void mouseMove(const Render::MouseEvent &event) override {
        _editor->vertexDragMove(event);
    }

    void hoverEnter(const Render::MouseEvent &event) override {
        _editor->_hoveredVertex = _vertex;
        _editor->updateWholeVAOs();

        switch (_editor->_appState->tool()) {
        case UI::Tool::Draw: {
            if (!_editor->_drawnVertices.empty()) {
                auto& lastVertex = _editor->_drawnVertices[_editor->_drawnVertices.size() - 1];
                if (lastVertex == _vertex) {
                    _editor->hoverMove(event);
                    return;
                }
            }
            break;
        }
        default: {
            break;
        }
        }
    }

    void hoverMove(const Render::MouseEvent &event) override {
        switch (_editor->_appState->tool()) {
        case UI::Tool::Draw: {
            if (!_editor->_drawnVertices.empty()) {
                auto& lastVertex = _editor->_drawnVertices[_editor->_drawnVertices.size() - 1];
                if (lastVertex == _vertex) {
                    _editor->hoverMove(event);
                    return;
                }
            }
            break;
        }
        default: {
            break;
        }
        }
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
        _editor->mousePressTarget({.edge = _edge}, event);
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
        _editor->mousePressTarget({.face = _face}, event);
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
    initializeOpenGLFunctions();
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
    Q_UNUSED(event);
}

void MeshEditor::mouseRelease(const Render::MouseEvent &event) {
    Q_UNUSED(event);
}

void MeshEditor::hoverMove(const Render::MouseEvent &event) {
    switch (_appState->tool()) {
    case UI::Tool::Draw: {
        auto mesh = _item->mesh();
        if (_drawnVertices.empty()) {
            break;
        } else {
            auto prevVert = _drawnVertices[_drawnVertices.size() - 1];

            auto [prevPosInScreen, isInScreen] = event.camera->mapWorldToScreen(prevVert->position());
            if (!isInScreen) {
                break;
            }
            auto pos = event.camera->mapScreenToWorld(dvec3(event.screenPos, prevPosInScreen.z));

            mesh->setPositions({{prevVert, pos}});
        }
        break;
    }
    default:
        break;
    }
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
    update();
}

void MeshEditor::mousePressTarget(const MeshEditor::EventTarget &target, const Render::MouseEvent &event) {
    switch (_appState->tool()) {
    case UI::Tool::Draw: {
        auto mesh = _item->mesh();
        if (_drawnVertices.empty()) {
            // TODO: better depth
            auto [centerInScreen, isCenterInScreen] = event.camera->mapWorldToScreen(vec3(0));
            if (!isCenterInScreen) {
                return;
            }
            auto pos = event.camera->mapScreenToWorld(dvec3(event.screenPos, centerInScreen.z));
            auto point1 = mesh->addUVPoint(mesh->addVertex(pos), vec2(0));
            auto point2 = mesh->addUVPoint(mesh->addVertex(pos), vec2(0));
            _drawnVertices.push_back(point1->vertex());
            _drawnVertices.push_back(point2->vertex());
            mesh->addEdge({point1->vertex(), point2->vertex()});

            return;
        }
        auto prevVert = _drawnVertices[_drawnVertices.size() - 1];
        if (target.vertex && target.vertex != prevVert) {
            mesh->addEdge({prevVert, *target.vertex});
            return;
        }

        auto [prevPosInScreen, isInScreen] = event.camera->mapWorldToScreen(prevVert->position());
        if (!isInScreen) {
            return;
        }
        auto pos = event.camera->mapScreenToWorld(dvec3(event.screenPos, prevPosInScreen.z));

        mesh->setPositions({{prevVert, pos}});

        auto uvPoint = mesh->addUVPoint(mesh->addVertex(pos), vec2(0));
        _drawnVertices.push_back(uvPoint->vertex());
        mesh->addEdge({prevVert, uvPoint->vertex()});

        return;
    }
    default: {
        LATTICE_OPTIONAL_LET(vertex, target.vertex, {
            vertexDragStart({vertex}, event);
        });
        LATTICE_OPTIONAL_LET(edge, target.edge, {
            vertexDragStart({edge->vertices()[0], edge->vertices()[1]}, event);
        });
        LATTICE_OPTIONAL_LET(face, target.face, {
            std::unordered_set<SP<Document::MeshVertex>> vertices;
            for (auto& v : face->vertices()) {
                vertices.insert(v);
            }
            vertexDragStart(vertices, event);
        });
        break;
    }
    }
}

void MeshEditor::mouseMoveTarget(const MeshEditor::EventTarget &target, const Render::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);
}

void MeshEditor::mouseReleaseTarget(const MeshEditor::EventTarget &target, const Render::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);
}

void MeshEditor::hoverEnterTarget(const MeshEditor::EventTarget &target, const Render::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);
}

void MeshEditor::hoverMoveTarget(const MeshEditor::EventTarget &target, const Render::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);
}

void MeshEditor::hoverLeaveTarget(const MeshEditor::EventTarget &target) {
    Q_UNUSED(target);
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
