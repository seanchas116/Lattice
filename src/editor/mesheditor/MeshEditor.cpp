#include "MeshEditor.hpp"
#include "MoveTool.hpp"
#include "DrawTool.hpp"
#include "ExtrudeTool.hpp"
#include "LoopCutTool.hpp"
#include "BorderSelectTool.hpp"
#include "../manipulator/MeshManipulator.hpp"
#include "../MeshVAOGenerator.hpp"
#include "../../state/AppState.hpp"
#include "../../state/MeshEditState.hpp"
#include "../../gl/VAO.hpp"
#include "../../gl/VertexBuffer.hpp"
#include "../../document/Document.hpp"
#include "../../document/History.hpp"
#include "../../document/MeshObject.hpp"
#include "../../mesh/Mesh.hpp"
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

// TODO: find beteter name
class EventTargetValue {
    static glm::vec4 encodeValueToColor(uint64_t value) {
        union {
            uint64_t value;
            glm::u16vec4 color;
        } valueColor;
        valueColor.value = value;

        return glm::vec4(valueColor.color) / float(0xFFFF);
    }

    static uint64_t decodeColorToValue(glm::vec4 color) {
        union {
            uint64_t value;
            glm::u16vec4 color;
        } valueColor;
        valueColor.color = glm::u16vec4(glm::round(color * float(0xFFFF)));
        return valueColor.value;
    }

    uint64_t value;

public:
    EventTargetValue(glm::vec4 hitUserColor) {
        value = decodeColorToValue(hitUserColor);
    }
    EventTargetValue() {
        value = 0;
    }
    EventTargetValue(Mesh::VertexHandle v) {
        value = v.index * 3 + 1;
    }
    EventTargetValue(Mesh::EdgeHandle e) {
        value = e.index * 3 + 2;
    }
    EventTargetValue(Mesh::FaceHandle f) {
        value = f.index * 3 + 3;
    }

    glm::vec4 hitUserColor() const {
        return encodeValueToColor(value);
    }

    Tool::EventTarget eventTarget() const {
        if (value == 0) {
            return {};
        }

        uint64_t type = (value - 1) % 3;
        uint64_t index = (value - 1) / 3;
        switch (type) {
        case 0:
            return {Mesh::VertexHandle(index), {}, {}};
        case 1:
            return {{}, Mesh::EdgeHandle(index), {}};
        case 2:
            return {{}, {}, Mesh::FaceHandle(index)};
        }
        return {};
    }
};

MeshEditor::MeshEditor(const SP<State::AppState>& appState, const SP<State::MeshEditState> &meshEditState) :
    _appState(appState),
    _meshEditState(meshEditState),
    _manipulator(makeShared<Manipulator::MeshManipulator>(meshEditState->object()->location().matrixToWorld(), meshEditState->mesh())),

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

    _tool(makeShared<MoveTool>(appState, meshEditState->object(), meshEditState->mesh()))
{
    updateVAOs();

    connect(appState.get(), &State::AppState::toolChanged, this, &MeshEditor::handleToolChange);
    handleToolChange(appState->tool());

    connect(_manipulator.get(), &Manipulator::Manipulator::onContextMenu, this, [this](auto& event) {
        contextMenuTarget({}, event);
    });
    connect(_manipulator.get(), &Manipulator::MeshManipulator::meshChanged, _meshEditState.get(), &State::MeshEditState::notifyMeshChange);
    connect(_manipulator.get(), &Manipulator::MeshManipulator::meshChangeFinished, this, [this] {
        _meshEditState->commitMeshChange(tr("Move Vertices"));
    });

    connect(appState.get(), &State::AppState::isTranslateHandleVisibleChanged, _manipulator.get(), &Manipulator::Manipulator::setTranslateHandleVisible);
    connect(appState.get(), &State::AppState::isRotateHandleVisibleChanged, _manipulator.get(), &Manipulator::Manipulator::setRotateHandleVisible);
    connect(appState.get(), &State::AppState::isScaleHandleVisibleChanged, _manipulator.get(), &Manipulator::Manipulator::setScaleHandleVisible);

    connect(meshEditState.get(), &State::MeshEditState::meshChanged, this, &MeshEditor::handleMeshChanged);
    connect(meshEditState->object().get(), &Document::MeshObject::subdivSettingsChanged, this, &MeshEditor::handleMeshChanged);
}

void MeshEditor::draw(const SP<Draw::Operations> &operations, const SP<Camera> &camera) {
    updateVAOs();

    // TODO: Render faces
    auto matrixToWorld = _meshEditState->object()->location().matrixToWorld();
    auto& materials = _meshEditState->object()->materials();

    for (auto& [materialID, vao] : _finalShapeVAOs) {
        auto material = materials.at(materialID).toDrawMaterial();
        operations->drawMaterial.draw(vao, matrixToWorld, camera, material);
    }

    operations->drawLine.draw(_edgeVAO, matrixToWorld, camera, 1.0, vec4(0), true);
    if (_appState->isVertexSelectable()) {
        operations->drawCircle.draw(_vertexVAO, matrixToWorld, camera, 6.0, vec4(0), true);
    }
}

void MeshEditor::drawHitArea(const SP<Draw::Operations> &operations, const SP<Camera> &camera) {
    updateVAOs();

    auto idColor = toIDColor();
    operations->clear.clear(idColor, 1);

    auto matrixToWorld = _meshEditState->object()->location().matrixToWorld();

    if (_appState->isFaceSelectable()) {
        operations->drawUnicolor.draw(_facePickVAO, matrixToWorld, camera, idColor, false);
    }
    if (_appState->isEdgeSelectable()) {
        operations->drawLine.draw(_edgePickVAO, matrixToWorld, camera, 12.0, idColor, false);
    }
    if (_appState->isVertexSelectable()) {
        operations->drawCircle.draw(_vertexPickVAO, matrixToWorld, camera, 24.0, idColor, false);
    }
}

void MeshEditor::drawHitUserColor(const SP<Draw::Operations> &operations, const SP<Camera> &camera) {
    updateVAOs();

    auto background = EventTargetValue().hitUserColor();
    operations->clear.clear(background, 1);

    auto matrixToWorld = _meshEditState->object()->location().matrixToWorld();

    if (_appState->isFaceSelectable()) {
        operations->drawUnicolor.draw(_facePickVAO, matrixToWorld, camera, vec4(0), true);
    }
    if (_appState->isEdgeSelectable()) {
        operations->drawLine.draw(_edgePickVAO, matrixToWorld, camera, 12.0, vec4(0), true);
    }
    if (_appState->isVertexSelectable()) {
        operations->drawCircle.draw(_vertexPickVAO, matrixToWorld, camera, 24.0, vec4(0), true);
    }
}

void MeshEditor::mousePressEvent(const Viewport::MouseEvent &event) {
    mousePressTarget(EventTargetValue(event.hitUserColor).eventTarget(), event);
}

void MeshEditor::mouseMoveEvent(const Viewport::MouseEvent &event) {
    mouseMoveTarget(EventTargetValue(event.hitUserColor).eventTarget(), event);
}

void MeshEditor::mouseReleaseEvent(const Viewport::MouseEvent &event) {
    mouseReleaseTarget(EventTargetValue(event.hitUserColor).eventTarget(), event);
}

void MeshEditor::contextMenuEvent(const Viewport::MouseEvent &event) {
    contextMenuTarget(EventTargetValue(event.hitUserColor).eventTarget(), event);
}

void MeshEditor::keyPressEvent(QKeyEvent *event) {
    _tool->keyPressTool(event);
}

void MeshEditor::keyReleaseEvent(QKeyEvent *event) {
    _tool->keyReleaseTool(event);
}

void MeshEditor::handleToolChange(State::Tool tool) {
    auto& object = _meshEditState->object();
    auto& mesh = _meshEditState->mesh();

    switch (tool) {
    case State::Tool::Draw:
        _tool = makeShared<DrawTool>(_appState, object, mesh);
        break;
    case State::Tool::Extrude:
        _tool = makeShared<ExtrudeTool>(_appState, object, mesh);
        break;
    case State::Tool::LoopCut:
        _tool = makeShared<LoopCutTool>(_appState, object, mesh);
        break;
    case State::Tool::BorderSelect:
        _tool = makeShared<BorderSelectTool>(_appState, object, mesh);
        break;
    default:
        _tool = makeShared<MoveTool>(_appState, object, mesh);
        break;
    }
    connect(_tool.get(), &Tool::meshChanged, this, &MeshEditor::handleMeshChanged);
    connect(_tool.get(), &Tool::meshChangeFinished, this, [this] (const QString& title) {
        _meshEditState->commitMeshChange(title);
        _appState->setTool(State::Tool::None);
    });
    updateManipulatorVisibility();
    updateChildren();
}

void MeshEditor::updateManipulatorVisibility() {
    auto& mesh = *_meshEditState->mesh();
    bool isVertexSelected = ranges::any_of(mesh.vertices(), [&] (auto v) { return mesh.isSelected(v); });
    _manipulator->setVisible(isVertexSelected  && _appState->tool() == State::Tool::None);
}

void MeshEditor::updateChildren() {
    setChildRenderables({_manipulator, _tool});
}

void MeshEditor::handleMeshChanged() {
    updateManipulatorVisibility();
    _manipulator->updatePosition();
    _isVAOsDirty = true;
    emit updated();
}

void MeshEditor::mousePressTarget(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    _tool->mousePressTool(target, event);
}

void MeshEditor::mouseMoveTarget(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    if (_lastMouseMoveTarget != target) {
        hoverLeaveTarget(_lastMouseMoveTarget);
        hoverEnterTarget(target, event);
    }
    _tool->mouseMoveTool(target, event);
}

void MeshEditor::mouseReleaseTarget(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    _tool->mouseReleaseTool(target, event);
}

void MeshEditor::hoverEnterTarget(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    _hoveredVertex = target.vertex;
    _hoveredEdge = target.edge;
    _hoveredFace = target.face;
    _isVAOsDirty = true;
    emit updated();

    _tool->hoverEnterTool(target, event);
}

void MeshEditor::hoverLeaveTarget(const Tool::EventTarget &target) {
    _hoveredVertex = {};
    _hoveredEdge = {};
    _hoveredFace = {};
    _isVAOsDirty = true;
    emit updated();

    _tool->hoverLeaveTool(target);
}

void MeshEditor::contextMenuTarget(const Tool::EventTarget &target, const Viewport::MouseEvent &event) {
    Q_UNUSED(target);

    QMenu contextMenu;
    contextMenu.addAction(tr("Delete Vertices"), _appState.get(), &State::AppState::deleteVertices);
    contextMenu.addAction(tr("Delete Edges"), _appState.get(), &State::AppState::deleteEdges);
    contextMenu.addAction(tr("Delete Faces"), _appState.get(), &State::AppState::deleteFaces);
    contextMenu.exec(event.originalContextMenuEvent->globalPos());
}

void MeshEditor::updateVAOs() {
    if (!_isVAOsDirty) {
        return;
    }

    auto& mesh = *_meshEditState->mesh();
    auto hitTestExclusion = _tool->hitTestExclusion();

    auto& object = _meshEditState->object();
    MeshVAOGenerator generator(mesh);
    if (object->subdivSettings().isEnabled) {
        _finalShapeVAOs = generator.generateSubdivFaceVAOs(object->subdivSettings().segmentCount);
    } else {
        _finalShapeVAOs = generator.generateFaceVAOs();
    }

    {
        std::vector<Draw::PointLineVertex> vertexAttributes;
        std::vector<Draw::PointLineVertex> vertexPickAttributes;

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

            Draw::PointLineVertex pickAttrib;
            pickAttrib.position = mesh.position(v);
            pickAttrib.color = EventTargetValue(v).hitUserColor();
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

            for (size_t vertexInEdgeIndex = 0; vertexInEdgeIndex < 2; ++vertexInEdgeIndex) {
                auto& v = mesh.vertices(e)[vertexInEdgeIndex];
                bool selected = mesh.isSelected(v);

                Draw::PointLineVertex attrib;
                attrib.position = mesh.position(v);
                attrib.color = hovered ? hoveredColor : selected ? selectedColor : unselectedColor;
                edgeAttributes.push_back(attrib);

                Draw::PointLineVertex pickAttrib;
                pickAttrib.position = mesh.position(v);
                pickAttrib.color = EventTargetValue(e).hitUserColor();
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
        facePickAttributes.reserve(_facePickVBO->size());
        faceTriangles.reserve(_faceIBO->size() / 3);

        for (auto f : mesh.faces()) {
            auto i0 = uint32_t(facePickAttributes.size());
            for (uint32_t i = 2; i < uint32_t(mesh.vertices(f).size()); ++i) {
                auto i1 = i0 + i - 1;
                auto i2 = i0 + i;
                faceTriangles.push_back({i0, i1, i2});
            }

            for (auto& p : mesh.uvPoints(f)) {
                Draw::Vertex pickAttrib;
                pickAttrib.position = mesh.position(mesh.vertex(p));
                pickAttrib.color = EventTargetValue(f).hitUserColor();
                facePickAttributes.push_back(pickAttrib);
            }
        }
        _facePickVBO->setVertices(facePickAttributes);
        _faceIBO->setTriangles(faceTriangles);
    }

    _isVAOsDirty = false;

    updateChildren();
}

}
