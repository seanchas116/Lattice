#include "MeshEditor.hpp"
#include "../../ui/AppState.hpp"
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

    MeshEditor* _editor;
    SP<Mesh::Face> _face;
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
    connect(_item->mesh().get(), &Mesh::Mesh::changed, this, &MeshEditor::updateWholeVAOs);
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
    mouseMoveTarget({}, event);
}

void MeshEditor::mouseRelease(const Render::MouseEvent &event) {
    mouseReleaseTarget({}, event);
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

            bool isDrawing = v == lastDrawnVertex();

            if (!isDrawing) {
                auto pickable = makeShared<VertexPickable>(this, v);
                childPickables.push_back(pickable);
                GL::Vertex pickAttrib;
                pickAttrib.position = v->position();
                pickAttrib.color = pickable->toIDColor();

                _vertexAttributes.push_back(attrib);
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

            bool isDrawing = e->vertices()[0] == lastDrawnVertex() || e->vertices()[1] == lastDrawnVertex();

            for (auto& v : e->vertices()) {
                bool selected = selectedVertices.find(v) != selectedVertices.end();

                GL::Vertex attrib;
                attrib.position = v->position();
                attrib.color = hovered ? hoveredColor : selected ? selectedColor : unselectedColor;
                _edgeAttributes.push_back(attrib);

                if (!isDrawing) {
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

        auto addPoint = [&](const SP<FacePickable>& pickable, const SP<Mesh::UVPoint>& p) {
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
    if (event.originalEvent->button() != Qt::LeftButton) {
        if (event.originalEvent->button() == Qt::RightButton) {
            QMenu contextMenu;
            contextMenu.addAction(tr("Delete Vertices"), _appState.get(), &UI::AppState::deleteVertices);
            contextMenu.addAction(tr("Delete Edges"), _appState.get(), &UI::AppState::deleteEdges);
            contextMenu.addAction(tr("Delete Faces"), _appState.get(), &UI::AppState::deleteFaces);
            contextMenu.exec(event.originalEvent->globalPos());
        }
        return;
    }

    switch (_appState->tool()) {
    case UI::Tool::Draw: {
        auto mesh = _item->mesh();
        if (!_drawnUVPoints.empty()) {
            if (target.vertex) {
                auto targetVertex = *target.vertex;

                mesh->removeVertex(_drawnUVPoints[_drawnUVPoints.size() - 1]->vertex());
                _drawnUVPoints.pop_back();

                // connect to existing vertex
                auto closingPointIt = std::find(_drawnUVPoints.begin(), _drawnUVPoints.end(), targetVertex->firstUVPoint());
                if (closingPointIt != _drawnUVPoints.end()) {
                    // create face
                    std::vector<SP<Mesh::UVPoint>> points(closingPointIt, _drawnUVPoints.end());
                    auto face = mesh->addFace(points, mesh->materials()[0]);

                    bool isFaceFore = dot(face->normal(), vec3(event.camera->location().backward())) > 0;
                    if (!isFaceFore) {
                        mesh->flipFace(face);
                    }

                    _drawnUVPoints.clear();
                    return;
                }
                auto prevUVPoint = _drawnUVPoints[_drawnUVPoints.size() - 1];
                mesh->addEdge({prevUVPoint->vertex(), targetVertex});
                _drawnUVPoints.push_back(targetVertex->firstUVPoint());

                auto uvPoint = mesh->addUVPoint(mesh->addVertex(targetVertex->position()), vec2(0));
                _drawnUVPoints.push_back(uvPoint);
                mesh->addEdge({targetVertex, uvPoint->vertex()});

            } else {
                // add new vertex
                auto prevUVPoint = _drawnUVPoints[_drawnUVPoints.size() - 1];
                auto [prevPosInScreen, isInScreen] = event.camera->mapWorldToScreen(prevUVPoint->vertex()->position());
                if (!isInScreen) {
                    return;
                }
                auto pos = event.camera->mapScreenToWorld(dvec3(event.screenPos, prevPosInScreen.z));

                mesh->setPositions({{prevUVPoint->vertex(), pos}});

                auto uvPoint = mesh->addUVPoint(mesh->addVertex(pos), vec2(0));
                _drawnUVPoints.push_back(uvPoint);
                mesh->addEdge({prevUVPoint->vertex(), uvPoint->vertex()});
            }
        } else {
            if (target.vertex) {
                // start from existing vertex
                auto& vertex = *target.vertex;
                auto point1 = (*vertex->uvPoints().begin())->sharedFromThis();
                auto point2 = mesh->addUVPoint(mesh->addVertex(vertex->position()), vec2(0));
                _drawnUVPoints.push_back(point1);
                _drawnUVPoints.push_back(point2);
                mesh->addEdge({point1->vertex(), point2->vertex()});

            } else {
                // start from new vertex
                // TODO: better depth
                auto [centerInScreen, isCenterInScreen] = event.camera->mapWorldToScreen(vec3(0));
                if (!isCenterInScreen) {
                    return;
                }
                auto pos = event.camera->mapScreenToWorld(dvec3(event.screenPos, centerInScreen.z));
                auto point1 = mesh->addUVPoint(mesh->addVertex(pos), vec2(0));
                auto point2 = mesh->addUVPoint(mesh->addVertex(pos), vec2(0));
                _drawnUVPoints.push_back(point1);
                _drawnUVPoints.push_back(point2);
                mesh->addEdge({point1->vertex(), point2->vertex()});
            }
        }
        return;
    }
    default: {
        if (target.vertex) {
            auto& vertex = *target.vertex;
            vertexDragStart({vertex}, event);
        } else if (target.edge) {
            auto& edge = *target.edge;
            vertexDragStart({edge->vertices()[0], edge->vertices()[1]}, event);
        } else if (target.face) {
            auto& face = *target.face;
            std::unordered_set<SP<Mesh::Vertex>> vertices;
            for (auto& v : face->vertices()) {
                vertices.insert(v);
            }
            vertexDragStart(vertices, event);
        }
        return;
    }
    }
}

void MeshEditor::mouseMoveTarget(const MeshEditor::EventTarget &target, const Render::MouseEvent &event) {
    switch (_appState->tool()) {
    case UI::Tool::Draw: {
        auto mesh = _item->mesh();
        auto maybePrevVertex = lastDrawnVertex();
        if (maybePrevVertex) {
            auto prevVertex = *maybePrevVertex;

            auto [prevPosInScreen, isInScreen] = event.camera->mapWorldToScreen(prevVertex->position());
            if (!isInScreen) {
                break;
            }
            auto pos = event.camera->mapScreenToWorld(dvec3(event.screenPos, prevPosInScreen.z));
            mesh->setPositions({{prevVertex, pos}});
        }
        return;
    }
    default: {
        if (target.vertex || target.edge || target.face) {
            vertexDragMove(event);
        }
        return;
    }
    }
}

void MeshEditor::mouseReleaseTarget(const MeshEditor::EventTarget &target, const Render::MouseEvent &event) {
    Q_UNUSED(target); Q_UNUSED(event);
    switch (_appState->tool()) {
    case UI::Tool::Draw: {
        return;
    }
    default: {
        vertexDragEnd();
        return;
    }
    }
}

void MeshEditor::hoverEnterTarget(const MeshEditor::EventTarget &target, const Render::MouseEvent &event) {
    Q_UNUSED(event);
    if (target.vertex) {
        _hoveredVertex = target.vertex;
        updateWholeVAOs(); // TODO: update partially
    } else if (target.edge) {
        _hoveredEdge = target.edge;
        updateWholeVAOs(); // TODO: update partially
    }
}

void MeshEditor::hoverLeaveTarget(const MeshEditor::EventTarget &target) {
    if (target.vertex) {
        _hoveredVertex = {};
        updateWholeVAOs(); // TODO: update partially
    } else if (target.edge) {
        _hoveredEdge = {};
        updateWholeVAOs(); // TODO: update partially
    }
}

void MeshEditor::vertexDragStart(const std::unordered_set<SP<Mesh::Vertex> > &vertices, const Render::MouseEvent &event) {
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

    _dragged = true;
    _dragInitPositions.clear();
    for (auto& v : selection.vertices) {
        _dragInitPositions[v] = v->position();
    }
    _dragInitWorldPos = event.worldPos();
    _dragStarted = false;

    _appState->document()->setMeshSelection(selection);
}

void MeshEditor::vertexDragMove(const Render::MouseEvent &event) {
    if (!_dragged) {
        return;
    }

    dvec3 worldPos = event.worldPos();
    dvec3 offset = worldPos - _dragInitWorldPos;

    if (!_dragStarted) {
        _appState->document()->history()->beginChange(tr("Move Vertex"));
        _dragStarted = true;
    }

    auto& mesh = _item->mesh();
    std::unordered_map<SP<Mesh::Vertex>, vec3> positions;
    for (auto& [v, initialPos] : _dragInitPositions) {
        positions[v] = initialPos + offset;
    }
    mesh->setPositions(positions);
}

void MeshEditor::vertexDragEnd() {
    _dragged = false;
}

Opt<SP<Mesh::UVPoint> > MeshEditor::lastDrawnPoint() const {
    if (_drawnUVPoints.empty()) {
        return {};
    }
    return _drawnUVPoints[_drawnUVPoints.size() - 1];
}

Opt<SP<Mesh::Vertex> > MeshEditor::lastDrawnVertex() const {
    auto uvPoint = lastDrawnPoint();
    if (!uvPoint) {
        return {};
    }
    return (*uvPoint)->vertex();
}

}
